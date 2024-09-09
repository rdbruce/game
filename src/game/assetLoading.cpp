#include "Game.hpp"
#include "../engine/LTexture.hpp"
#include "../engine/LWindow.hpp"
#include "../engine/LAudio.hpp"


void Game::load_levels( std::string dir )
{
    Base = Scene(dir + "Base.txt", this);
    if (Base.player != nullptr) currLevel = &Base;

    Woods = Scene(dir + "Woods.txt", this);
    if (Woods.player != nullptr) currLevel = &Woods;

    Town = Scene(dir + "Town.txt", this);
    if (Town.player != nullptr) currLevel = &Town;

    // set up pointers to adjacent levels
    Base.assignNeighbours(&Town, &Woods);
    Woods.assignNeighbours(&Base, nullptr);
    Town.assignNeighbours(nullptr, &Base);

    isNight = false;
    g_time = 0.0f;

    interactRange = 3.0f * currLevel->cell_sideLen;

    load_gameData(dir + "gameData.txt");
}

void Game::load_gameData( std::string filename )
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Couldn't open " << filename << std::endl;
    } else {
        std::string line;
        std::getline(file, line);

        std::istringstream iss( line );

        iss >> std::dec >> isNight >> riverDammed >> enemySpawnRate;
        file.close();
    }
}

void Game::load_textures()
{
    fpsTex = std::make_unique<LTexture>(window);
    controlsTex = std::make_unique<LTexture>(window);
    // load all the textures from their image files

    grassTex = std::make_shared<LTexture>(window);
    if (!grassTex->loadFromFile("../../assets/Ground/Grass.png")) {
        std::cerr << "Failed to load grass texture!" << std::endl;
    }

    logTex = std::make_shared<LTexture>(window);
    if (!logTex->loadFromFile("../../assets/Buildings/Log.png")) {
        std::cerr << "Failed to load log texture!" << std::endl;
    }

    damTex = std::make_shared<LTexture>(window);
    if (!damTex->loadFromFile("../../assets/Buildings/Dam.png")) {
        std::cerr << "Failed to load DAM texture!" <<std::endl; 
    }

    waterTex = std::make_shared<LTexture>(window);
    if (!waterTex->loadFromFile("../../assets/Ground/Water.png")) {
        std::cerr << "Failed to load water texture!" << std::endl;
    }

    treeTex = std::make_shared<LTexture>(window);
    if (!treeTex->loadFromFile("../../assets/Tree/Tree.png")) {
        std::cerr << "Failed to load tree texture!" << std::endl;
    }

    stumpTex = std::make_shared<LTexture>(window);
    if (!stumpTex->loadFromFile("../../assets/Tree/Stump.png")) {
        std::cerr << "Failed to load stump texture!" << std::endl;
    }

    saplingTex = std::make_shared<LTexture>(window);
    if (!saplingTex->loadFromFile("../../assets/Tree/Sapling.png")) {
        std::cerr << "Failed to load sapling texture!" << std::endl;
    }

    shoreline0Tex = std::make_shared<LTexture>(window);
    if (!shoreline0Tex->loadFromFile("../../assets/Ground/Shoreline0.png")) {
        std::cerr << "Failed to load shoreline 0 texture!" << std::endl;
    }
    shoreline1Tex = std::make_shared<LTexture>(window);
    if (!shoreline1Tex->loadFromFile("../../assets/Ground/Shoreline1.png")) {
        std::cerr << "Failed to load shoreline 1 texture!" << std::endl;
    }
    shoreline2Tex = std::make_shared<LTexture>(window);
    if (!shoreline2Tex->loadFromFile("../../assets/Ground/Shoreline2.png")) {
        std::cerr << "Failed to load shoreline 2 texture!" << std::endl;
    }
    shoreline3Tex = std::make_shared<LTexture>(window);
    if (!shoreline3Tex->loadFromFile("../../assets/Ground/Shoreline3.png")) {
        std::cerr << "Failed to load shoreline 3 texture!" << std::endl;
    }
    shoreline4Tex = std::make_shared<LTexture>(window);
    if (!shoreline4Tex->loadFromFile("../../assets/Ground/Shoreline4.png")) {
        std::cerr << "Failed to load shoreline 4 texture!" << std::endl;
    }
    closed_doorTex = std::make_shared<LTexture>(window);
    if (!closed_doorTex->loadFromFile("../../assets/Buildings/Door.png")) {
        std::cerr << "Failed to load closed door texture!" << std::endl;
    }
    open_doorTex = std::make_shared<LTexture>(window);
    if (!open_doorTex->loadFromFile("../../assets/Buildings/Open_Door.png")) {
        std::cerr << "Failed to load open door texture!" << std::endl;
    }
    playerTex = std::make_shared<LTexture>(window);
    if (!playerTex->loadFromFile("../../assets/Entities/Player/front/0.png")) {
        std::cerr << "Failed to load texture for player!" << std::endl;
    }
    wolfTex = std::make_shared<LTexture>(window);
    if (!wolfTex->loadFromFile("../../assets/Entities/Wolf.png")) {
        std::cerr << "Failed to load texture for wolf!" << std::endl;
    }
    falling_treeTex = std::make_shared<LTexture>(window);
    if (!falling_treeTex->loadFromFile("../../assets/Tree/Falling_Tree.png")) {
        std::cerr << "Failed to load texture for falling tree!" << std::endl;
    }
    pine_coneTex = std::make_shared<LTexture>(window);
    if (!pine_coneTex->loadFromFile("../../assets/Items/Pine_Cone.png")) {
        std::cerr << "Failed to load texture for pine cone!" << std::endl;
    }
    plankTex = std::make_shared<LTexture>(window);
    if (!plankTex->loadFromFile("../../assets/Items/Plank.png")) {
        std::cerr << "Failed to load texture for plank!" << std::endl;
    }

    foxTex = std::make_shared<LTexture>(window);
    if (!foxTex->loadFromFile("../../assets/Entities/Fox.png")) {
        std::cerr << "Failed to load texture for fox!" << std::endl;
    }
    berryTex = std::make_shared<LTexture>(window);
    if (!berryTex->loadFromFile("../../assets/Items/Berry.png")) {
        std::cerr << "Failed to load texture for berry!" << std::endl;
    }
    dirtTex = std::make_shared<LTexture>(window);
    if (!dirtTex->loadFromFile("../../assets/Ground/Dirt.png")) {
        std::cerr << "Failed to load texture for dirt!" << std::endl;
    }
    berry_bushTex = std::make_shared<LTexture>(window);
    if (!berry_bushTex->loadFromFile("../../assets/Buildings/Berry_Bush.png")) {
        std::cerr << "Failed to load texture for berry bush!" << std::endl;
    }
    empty_bushTex = std::make_shared<LTexture>(window);
    if (!empty_bushTex->loadFromFile("../../assets/Buildings/EmptyBush.png")) {
        std::cerr << "Failed to load texture for depleted bush!" << std::endl;
    }
    heartTex = std::make_shared<LTexture>(window);
    if (!heartTex->loadFromFile("../../assets/Menu/Heart.png")) {
        std::cerr << "Failed to load texture for heart!" << std::endl;
    }
    stoneTex = std::make_shared<LTexture>(window);
    if (!stoneTex->loadFromFile("../../assets/Items/Stone.png")) {
        std::cerr << "Failed  to load texture for stone!" << std::endl;
    }
    LMBTex = std::make_shared<LTexture>(window);
    if (!LMBTex->loadFromFile("../../assets/Menu/Mouse/LeftClick.png")) {
        std::cerr << "failed to load left mouse texture!" << std::endl;
    }
    MMBTex = std::make_shared<LTexture>(window);
    if (!MMBTex->loadFromFile("../../assets/Menu/Mouse/MiddleClick.png")) {
        std::cerr << "failed to load middle mouse texture!" << std::endl;
    }
    RMBTex = std::make_shared<LTexture>(window);
    if (!RMBTex->loadFromFile("../../assets/Menu/Mouse/RightClick.png")) {
        std::cerr << "failed to load right mouse texture!" << std::endl;
    }
    BearTex = std::make_shared<LTexture>(window);
    if (!BearTex->loadFromFile("../../assets/Entities/Bear.png")) {
        std::cerr << "Failed to load bear texture!" << std::endl;
    }
    BirdTex = std::make_shared<LTexture>(window);
    if (!BirdTex->loadFromFile("../../assets/Entities/Bird.png")) {
        std::cerr << "Failed to load texture for bird!" << std::endl;
    }
    BombTex = std::make_shared<LTexture>(window);
    if (!BombTex->loadFromFile("../../assets/Entities/Bomb.png")) {
        std::cerr << "Failed to load texture for bomb!" << std::endl;
    }
    TargetTex = std::make_shared<LTexture>(window);
    if (!TargetTex->loadFromFile("../../assets/Target.png")) {
        std::cerr << "Failed to load texture for target!" << std::endl;
    }
    dashed_circleTex = std::make_shared<LTexture>(window);
    if (!dashed_circleTex->loadFromFile("../../assets/DashedCircle.png")) {
        std::cerr << "Failed to load texture for explosion indicator!" << std::endl;
    }
    rabbitTex = std::make_shared<LTexture>(window);
    if (!rabbitTex->loadFromFile("../../assets/Entities/Rabbit.png")) {
        std::cerr << "Failed to load texture for rabbit!" << std::endl;
    }
}

void Game::load_audio()
{
    logDestruction = std::make_shared<LAudio>();
    if (!logDestruction->loadFromFile("../../assets/Audio/Destruction/LogBreaking.wav")) {
        std::cerr << "Failed to load audio for log breaking!" << std::endl;
    }

    treeFalling = std::make_shared<LAudio>();
    if (!treeFalling->loadFromFile("../../assets/Audio/Destruction/TreeFalling.wav")) {
        std::cerr << "Failed to load audio for tree falling!" << std::endl;
    }

    doorToggle = std::make_shared<LAudio>();
    if (!doorToggle->loadFromFile("../../assets/Audio/DoorToggle.wav")) {
        std::cerr << "Failed to load audio for door opening!" << std::endl;
    }
    
    leaves = std::make_shared<LAudio>();
    if (!leaves->loadFromFile("../../assets/Audio/Destruction/Leaves.wav")) {
        std::cerr << "Failed to load audio for leaves!" << std::endl;
    }

    pop = std::make_shared<LAudio>();
    if (!pop->loadFromFile("../../assets/Audio/EntitySounds/Pop.wav")) {
        std::cerr << "Failed to load item pop sound!" << std::endl;
    }

    bonk = std::make_shared<LAudio>();
    if (!bonk->loadFromFile("../../assets/Audio/EntitySounds/Bonk.wav")) {
        std::cerr << "Failed to load bonk sound!" << std::endl;
    }
    birdSpawn = std::make_shared<LAudio>();
    if (!birdSpawn->loadFromFile("../../assets/Audio/EntitySounds/BirdSpawn.wav")) {
        std::cerr << "Failed to load bird spawn sound!" << std::endl;
    }
    gameOverSound = std::make_shared<LAudio>();
    if (!gameOverSound->loadFromFile("../../assets/Audio/GameOver.wav")) {
        std::cerr << "Failed to load bird spawn sound!" << std::endl;
    }
    healSound = std::make_shared<LAudio>();
    if (!healSound->loadFromFile("../../assets/Audio/EntitySounds/Heal.wav")) {
        std::cerr << "Failed to load bird spawn sound!" << std::endl;
    }
    playerDamage = std::make_shared<LAudio>();
    if (!playerDamage->loadFromFile("../../assets/Audio/EntitySounds/PlayerDamage.wav")) {
        std::cerr << "Failed to load bird spawn sound!" << std::endl;
    }
    explosionSound = std::make_shared<LAudio>();
    if (!explosionSound->loadFromFile("../../assets/Audio/Destruction/Explosion.wav")) {
        std::cerr << "Failed to load explosion sound!" << std::endl;
    }

    std::string filename = "../../assets/Audio/Speech/8bit_vocal_deep0.wav";
    vocalDeep0 = std::make_shared<LAudio>();
    if (!vocalDeep0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_deep1.wav";
    vocalDeep1 = std::make_shared<LAudio>();
    if (!vocalDeep1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_med0.wav";
    vocalMed0 = std::make_shared<LAudio>();
    if (!vocalMed0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_med1.wav";
    vocalMed1 = std::make_shared<LAudio>();
    if (!vocalMed1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_high0.wav";
    vocalHigh0 = std::make_shared<LAudio>();
    if (!vocalHigh0->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Speech/8bit_vocal_high1.wav";
    vocalHigh1 = std::make_shared<LAudio>();
    if (!vocalHigh1->loadFromFile(filename)) {
        std::cerr << "Failed to load " << filename << std::endl;
    }
    filename = "../../assets/Audio/Repair.wav";
    repairSound = std::make_shared<LAudio>();
    if (!repairSound->loadFromFile(filename)) {
        std::cerr << "Failed to laod " << filename << std::endl;
    }
    

    for (int i = 0; i < 3; i++) {
        std::string filename = "../../assets/Audio/EntitySounds/WolfSpawn"+std::to_string(i+1)+".wav";
        auto sound = std::make_shared<LAudio>();
        if (sound->loadFromFile(filename)) {
            wolfSpawnSounds[i] = sound;
        } else {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }
    
    for (int i = 0; i < 3; i++) {
        std::string filename = "../../assets/Audio/Destruction/splash/"+std::to_string(i)+".wav";
        auto sound = std::make_shared<LAudio>();
        if (sound->loadFromFile(filename)) {
            splashSounds[i] = sound;
        } else {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }


    nightMusic = std::make_shared<LAudio>();
    if (!nightMusic->loadFromFile("../../assets/Audio/Music/digiworld demigirl.wav")) {
        std::cerr << "failed to load digiworld demigirl.wav!" << std::endl;
    }
    dayMusic = std::make_shared<LAudio>();
    if (!dayMusic->loadFromFile("../../assets/Audio/Music/Arp 9.wav")) {
        std::cerr << "Failed to load Arp 9.wav!" << std::endl;
    }
    menuMusic = std::make_shared<LAudio>();
    if (!menuMusic->loadFromFile("../../assets/Audio/Music/acidruinedyourlifecore.wav")) {
        std::cerr << "Failed to load acidruinedyourlifecore.wav!" << std::endl;
    }
    deathMusic = std::make_shared<LAudio>();
    if (!deathMusic->loadFromFile("../../assets/Audio/Music/internetboy.wav")) {
        std::cerr << "Failed to load internetboy.wav!" << std::endl;
    }
}

void Game::load_fonts()
{
    sevenSegment = TTF_OpenFont("../../assets/Fonts/Seven_Segment.ttf", 48);
    if (sevenSegment == NULL) {
        std::cerr << "Failed to load seven segment font!" << std::endl;
    }

    arcadeClassic24 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 24);
    if (arcadeClassic24 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }
    arcadeClassic36 = TTF_OpenFont("../../assets/Fonts/ARCADECLASSIC.TTF", 36);
    if (arcadeClassic36 == NULL) {
        std::cerr << "Failed to load arcade classic font!" << std::endl;
    }
}

void Game::load_animations()
{
    // player animations
    std::vector<std::shared_ptr<LTexture>> front, back, left, right;
    float df = 1.0f, db = 1.0f, dl = 1.0f, dr = 1.0f;

    std::string path = "../../assets/Entities/Player/";
    std::string path1 = path + "back/";
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        back.push_back(newTex);
    }

    path1 = path + "front/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }
    
    path1 = path + "left/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        left.push_back(newTex);
    }
    
    path1 = path + "right/";    
    for (int i = 0; i < 4; i++) 
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        right.push_back(newTex);
    }

    playerAnimations = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);


    // wolf walking animations
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 1.0f, db = 1.0f, dl = 1.0f, dr = 1.0f;
    path = "../../assets/Entities/Wolf/Walking/";
    
    path1 = path + "forwards/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }
    
    path1 = path + "backwards/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        back.push_back(newTex);
    }
    
    path1 = path + "left/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        left.push_back(newTex);
    }
    
    path1 = path + "right/";
    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path1 + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        right.push_back(newTex);
    }

    wolfWalkingAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);


    // explosion
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 0.25f, db = 0.25f, dl = 0.25f, dr = 0.25f;
    path = "../../assets/Entities/Explosion/";

    for (int i = 0; i < 4; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }

    explosionAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);
    
    // splash
    front.clear(); back.clear(); left.clear(); right.clear();
    df = 0.3333f, db = 0.3333f, dl = 0.3333f, dr = 0.3333f;
    path = "../../assets/Entities/splash/";

    for (int i = 0; i < 3; i++)
    {
        auto newTex = std::make_shared<LTexture>(window);
        std::string filename = path + std::to_string(i) + ".png";
        if (!newTex->loadFromFile(filename)) break;
        front.push_back(newTex);
    }

    splashAnimation = std::make_shared<Animations>(front, back, left, right, df, db, dl, dr);
}

void Game::create_textures()
{
    clockBackBar = tEditor.createSolidColour(75, 5, 0x000000D0, window);
    redBar = tEditor.createSolidColour(81, 11, 0xFF0000D0, window);
    clockTex = std::make_unique<LTexture>(window);
}