////////////////////////////////////////////////////////////////////////////////
/////////// Author: Andres Zibula                                   ////////////
/////////// Source: https://github.com/andres-zibula/ogre-3d-game   ////////////
////////////////////////////////////////////////////////////////////////////////

#include "Core.h"

Core::Core(void) : logManager(0), raiz(0), ventana(0), gestorEscena(0), gestorEntrada(0), teclado(0), mouse(0), myGUIPlatform(0), myGUI(0)
{
	//si no hubo problemas en la configuracion de OGRE
	if(configurarOgre())
	{
		ventana = raiz->initialise(true);
		gestorEscena = raiz->createSceneManager(Ogre::ST_GENERIC, "gestorEscena");

		#ifdef _WIN32
            gestorEscena->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		#endif

		configurarOIS();
		configurarCamara();
		cargarRecursos();
		configurarGUI();
	}
}

Core::~Core(void)
{
	if(gestorEntrada)
	{
		if(teclado)
			gestorEntrada->destroyInputObject(teclado);
		if(mouse)
		{
			gestorEntrada->destroyInputObject(mouse);
		}

		OIS::InputManager::destroyInputSystem(gestorEntrada);
	}

	if(myGUI)
	{
		myGUI->shutdown();
		delete myGUI;
		myGUI = 0;
	}

	if(myGUIPlatform)
	{
		myGUIPlatform->shutdown();
		delete myGUIPlatform;
		myGUIPlatform = 0;
	}

	if(gestorEscena)
		gestorEscena->clearScene();

	if(ventana)
		ventana->removeAllViewports();

	if(gestorEscena)
		gestorEscena->destroyCamera(camara);

	if(raiz && gestorEscena)
		raiz->destroySceneManager(gestorEscena);

	if(raiz)
	{
		delete raiz;
		raiz = 0;
	}

	if(logManager)
	{
		delete logManager;
		logManager = 0;
	}

	int asd = 0;
	if(asd)
	{
		Mix_CloseAudio();
		SDL_Quit();
	}

}

bool Core::configurarOgre(void)
{
	logManager = new Ogre::LogManager();
	logSistema = logManager->createLog("logSistema.txt", true);
	logJuego = logManager->createLog("logJuego.txt");

	raiz = new Ogre::Root("", "config.cfg");


	#ifdef _DEBUG
		#ifdef _WIN32
			raiz->loadPlugin("RenderSystem_Direct3D9_d");
			raiz->loadPlugin("Plugin_ParticleFX_d");
			//raiz->loadPlugin("Plugin_CgProgramManager_d");
		#elif __linux__
			raiz->loadPlugin("/usr/local/lib/OGRE/RenderSystem_GL");
			raiz->loadPlugin("/usr/local/lib/OGRE/Plugin_ParticleFX");
			//raiz->loadPlugin("/usr/local/lib/OGRE/Plugin_CgProgramManager");
		#endif
	#else
		#ifdef _WIN32
			raiz->loadPlugin("RenderSystem_Direct3D9");
			raiz->loadPlugin("Plugin_ParticleFX");
			//raiz->loadPlugin("Plugin_CgProgramManager");
		#elif __linux__
			raiz->loadPlugin("/usr/local/lib/OGRE/RenderSystem_GL");
			raiz->loadPlugin("/usr/local/lib/OGRE/Plugin_ParticleFX");
			//raiz->loadPlugin("/usr/local/lib/OGRE/Plugin_CgProgramManager");
		#endif
	#endif

	//si fallamos al restaurar el archivo de configuracion
	if(!raiz->restoreConfig())
	{
		//si el usuario cancela el dialogo
		if(!raiz->showConfigDialog())
		{
			delete raiz;
			raiz = 0;
			return false;
		}
	}

	return true;
}

void Core::configurarOIS(void)
{
	hndVentana = 0;
	ventana->getCustomAttribute("WINDOW", &hndVentana);
	strHndVentana << hndVentana;
	listaParamOIS.insert(std::make_pair(std::string("WINDOW"), strHndVentana.str()));

	#if defined OIS_WIN32_PLATFORM
	listaParamOIS.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	listaParamOIS.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	listaParamOIS.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	listaParamOIS.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	#elif defined OIS_LINUX_PLATFORM
	listaParamOIS.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
	listaParamOIS.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
	listaParamOIS.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	listaParamOIS.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
	#endif

	gestorEntrada = OIS::InputManager::createInputSystem(listaParamOIS);

	teclado = static_cast<OIS::Keyboard*>(gestorEntrada->createInputObject(OIS::OISKeyboard, true));
	mouse = static_cast<OIS::Mouse*>(gestorEntrada->createInputObject(OIS::OISMouse, true));
}

void Core::configurarCamara()
{
	camara = gestorEscena->createCamera("camara");
	camara->setNearClipDistance(5);
	camara->setFarClipDistance(10000);
	viewport = ventana->addViewport(camara);
	camara->setAspectRatio(viewport->getWidth() / viewport->getHeight());
	viewport->setBackgroundColour(Ogre::ColourValue(1.0, 1.0, 1.0));
}

void Core::cargarRecursos(void)
{
	archivoRecursosConfig.load("recursos.cfg");

	Ogre::ConfigFile::SectionIterator iteradorGrupo = archivoRecursosConfig.getSectionIterator();

	Ogre::String nombreGrupo, nombreTipo, nombreArchivo;

	while(iteradorGrupo.hasMoreElements())
	{
		nombreGrupo = iteradorGrupo.peekNextKey();

		Ogre::ConfigFile::SettingsMultiMap* opciones = iteradorGrupo.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for(it = opciones->begin(); it != opciones->end(); ++it)
		{
			nombreTipo = it->first;
			nombreArchivo = it->second;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(nombreArchivo, nombreTipo, nombreGrupo);
		}
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");

	/*Ogre::Entity* entidad1 = gestorEscena->createEntity("asd", "Sinbad.mesh");
	Ogre::Entity* entidad2 = gestorEscena->createEntity("asd2", "ninja.mesh");
	Ogre::Entity* entidad3 = gestorEscena->createEntity("asd3", "demonio.mesh");
	Ogre::Entity* entidad4 = gestorEscena->createEntity("asd4", "goblin.mesh");
	Ogre::Entity* entidad5 = gestorEscena->createEntity("asd5", "golem.mesh");
	Ogre::Entity* entidad6 = gestorEscena->createEntity("asd6", "Sword.mesh");
	Ogre::Entity* entidad7 = gestorEscena->createEntity("asd7", "Cube.001.mesh");*/
}

void Core::configurarGUI()
{
	myGUIPlatform = new MyGUI::OgrePlatform();
	myGUIPlatform->initialise(ventana, gestorEscena);
	myGUI = new MyGUI::Gui();
	myGUI->initialise();

	const OIS::MouseState &mouseState = mouse->getMouseState();
	mouseState.width = ventana->getWidth();
	mouseState.height = ventana->getHeight();
}

void Core::configurarSonido(void)
{
	/*SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);

	Mix_Music* musica = Mix_LoadMUS("path");
	Mix_PlayMusic(musica, -1);
	Mix_HaltMusic();
	Mix_FreeMusic(musica);
	Mix_Chunk* sonido = Mix_LoadWAV("patth");
	Mix_FreeChunk(sonido);*/
}
