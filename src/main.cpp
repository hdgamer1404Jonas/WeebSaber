#include "main.hpp"

#include "assets.hpp"
#include "images.hpp"
#include "static.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/HealthWarningViewController.hpp"
#include "GlobalNamespace/BasicUIAudioManager.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/GameObject.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/Touchable.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Space.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/AudioSource.hpp"

#include <cstdint>
#include <functional>


using namespace QuestUI::BeatSaberUI;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup



// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

#include "UnityEngine/WaitForSeconds.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "GlobalNamespace/BloomPrePassBackgroundColor.hpp"
#include "GlobalNamespace/CampaignFlowCoordinator.hpp"
#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/PartyFreePlayFlowCoordinator.hpp"
#include "GlobalNamespace/SoloFreePlayFlowCoordinator.hpp"
#include "GlobalNamespace/MenuLightsPresetSO.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/MenuLightsManager.hpp"
#include "GlobalNamespace/MenuLightsPresetSO_LightIdColorPair.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"

#include "UnityEngine/ScriptableObject.hpp"
#include "UnityEngine/Object.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Color.hpp"

#include <map>

using namespace GlobalNamespace;

MenuLightsPresetSO *defaultLights = nullptr;

GlobalNamespace::ColorSO *createColorSO(UnityEngine::Color color) {
    float t = color.r + color.g + color.b;
    auto so = UnityEngine::ScriptableObject::CreateInstance<
            GlobalNamespace::SimpleColorSO *>();
    so->color = color;
    return so;
}

GlobalNamespace::MenuLightsPresetSO *createMenuLights(UnityEngine::Color color) {
    float t = color.r + color.g + color.b;
    auto colorSO = createColorSO(color);
    auto menuPresetSO = UnityEngine::Object::Instantiate(defaultLights);
    auto colorPairs = menuPresetSO->lightIdColorPairs;
    for (int i = 0; i < colorPairs.Length(); ++i) {
        auto pair = GlobalNamespace::MenuLightsPresetSO::LightIdColorPair::
        New_ctor();
        pair->lightId = colorPairs[i]->lightId;
        pair->baseColor = colorSO;
        pair->intensity = colorPairs[i]->intensity;
        colorPairs[i] = pair;
    }
    return menuPresetSO;
}

#include "UnityEngine/Rigidbody.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Light.hpp"
#include "UnityEngine/BoxCollider.hpp"

void createCube(UnityEngine::Vector3 position, UnityEngine::Vector3 scale, UnityEngine::Color color) {
    auto *cube = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Cube);
    auto *rigidbody = cube->AddComponent<UnityEngine::Rigidbody *>();
    rigidbody->set_useGravity(false);
    rigidbody->set_isKinematic(true);
    cube->get_transform()->set_position(position);
    cube->get_transform()->set_localScale(scale);
    auto *meshRenderer = cube->GetComponent<UnityEngine::MeshRenderer *>();
    auto *material = meshRenderer->get_material();
    material->set_color(color);
    auto *light = cube->AddComponent<UnityEngine::Light *>();
    light->set_color(color);
    light->set_intensity(1);
    light->set_intensity(1);

    // make it sp the cube can be picked up by putting the controller into it and pressing the grip button
    auto *collider = cube->AddComponent<UnityEngine::BoxCollider *>();
    collider->set_isTrigger(true);

    // make it so the cube can be picked up by putting the controller into it and pressing the grip button
    auto *touchable = cube->AddComponent<HMUI::Touchable *>();
    touchable->set_enabled(true);

    // add uwu on all sides of the cube
    auto *uwu = QuestUI::BeatSaberUI::CreateText(cube->get_transform(), "UwU", false);
    uwu->set_fontSize(5);
    uwu->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(0, 0));
    uwu->get_rectTransform()->set_anchoredPosition3D(UnityEngine::Vector3(0, 0, 0));
    uwu->get_rectTransform()->set_localScale(UnityEngine::Vector3(0.1, 0.1, 0.1));
}


#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/RenderSettings.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Mathf.hpp"

UnityEngine::GameObject* backgroundObject;
UnityEngine::Material* backgroundMat;
UnityEngine::Texture* backgroundTexture;

MAKE_HOOK_MATCH(MainMenuUIHook, &GlobalNamespace::MainMenuViewController::DidActivate, void,
                GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MainMenuUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        // get the image of the solo button
        UnityEngine::GameObject* soloButton = self->soloButton->get_gameObject();
        auto* buttonSpriteSwap = soloButton->GetComponentInChildren<HMUI::ButtonSpriteSwap*>();
        UnityEngine::Sprite* soloSprite = Base64ToSprite(solo_image);
        soloSprite->set_name(il2cpp_utils::newcsstr("soloSprite"));
        buttonSpriteSwap->normalStateSprite = soloSprite;
        buttonSpriteSwap->highlightStateSprite = soloSprite;
        buttonSpriteSwap->pressedStateSprite = soloSprite;
        buttonSpriteSwap->disabledStateSprite = soloSprite;
        auto* soloGameObject = soloButton->get_gameObject();
        auto* soloMenuText = soloGameObject->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
        soloMenuText->set_text("Solo UwU~");


        UnityEngine::GameObject* multiplayerButton = self->multiplayerButton->get_gameObject();
        auto* multiplayerButtonSpriteSwap = multiplayerButton->GetComponentInChildren<HMUI::ButtonSpriteSwap*>();
        UnityEngine::Sprite* multiplayerSprite = Base64ToSprite(multi_image);
        multiplayerSprite->set_name(il2cpp_utils::newcsstr("multiplayerSprite"));
        multiplayerButtonSpriteSwap->normalStateSprite = multiplayerSprite;
        multiplayerButtonSpriteSwap->highlightStateSprite = multiplayerSprite;
        multiplayerButtonSpriteSwap->pressedStateSprite = multiplayerSprite;
        multiplayerButtonSpriteSwap->disabledStateSprite = multiplayerSprite;
        auto* multiplayerGameObject = multiplayerButton->get_gameObject();
        auto* multiplayerMenuText = multiplayerGameObject->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
        multiplayerMenuText->set_text("Multiplayer UwU~");

        UnityEngine::GameObject* campaignButton = self->campaignButton->get_gameObject();
        auto* campaignButtonSpriteSwap = campaignButton->GetComponentInChildren<HMUI::ButtonSpriteSwap*>();
        UnityEngine::Sprite* campaignSprite = Base64ToSprite(campaign_image);
        campaignSprite->set_name(il2cpp_utils::newcsstr("campaignSprite"));
        campaignButtonSpriteSwap->normalStateSprite = campaignSprite;
        campaignButtonSpriteSwap->highlightStateSprite = campaignSprite;
        campaignButtonSpriteSwap->pressedStateSprite = campaignSprite;
        campaignButtonSpriteSwap->disabledStateSprite = campaignSprite;
        auto* campaignGameObject = campaignButton->get_gameObject();
        auto* campaignMenuText = campaignGameObject->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
        campaignMenuText->set_text("Campaign UwU~");

        UnityEngine::GameObject* groupButton = self->partyButton->get_gameObject();
        auto* groupButtonSpriteSwap = groupButton->GetComponentInChildren<HMUI::ButtonSpriteSwap*>();
        UnityEngine::Sprite* groupSprite = Base64ToSprite(group_image);
        groupSprite->set_name(il2cpp_utils::newcsstr("groupSprite"));
        groupButtonSpriteSwap->normalStateSprite = groupSprite;
        groupButtonSpriteSwap->highlightStateSprite = groupSprite;
        groupButtonSpriteSwap->pressedStateSprite = groupSprite;
        groupButtonSpriteSwap->disabledStateSprite = groupSprite;
        auto* groupGameObject = groupButton->get_gameObject();
        auto* groupMenuText = groupGameObject->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();
        groupMenuText->set_text("Group UwU~");


        auto text = QuestUI::BeatSaberUI::CreateText(self->get_transform(), getUwUQuote(), false);
        text->set_fontSize(10);
        // place it at the top of the menu buttons
        text->get_rectTransform()->set_anchoredPosition(UnityEngine::Vector2(-30, 40));


        auto* mainFlow = QuestUI::BeatSaberUI::GetMainFlowCoordinator();

        if(!defaultLights) {
            defaultLights = mainFlow->menuLightsManager->defaultPreset;
        }

        auto mfc = mainFlow->menuLightsManager;

        auto soloFreeplay = mainFlow->soloFreePlayFlowCoordinator;
        auto partyFreeplay = mainFlow->partyFreePlayFlowCoordinator;
        auto campaignCoordinator = mainFlow->campaignFlowCoordinator;

        // pink color
        UnityEngine::Color pink = UnityEngine::Color(1.0f, 0.0f, 1.0f, 1.0f);

        mainFlow->defaultLightsPreset = createMenuLights(pink);
        mfc->SetColorPreset(mainFlow->defaultLightsPreset, true);


        createCube(UnityEngine::Vector3(0, 0, 0), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);

        int cubeCount = 0;

        // make a circle of cubes around the player
        for (int i = 0; i < 360; i += 5) {
            float x = 5 * UnityEngine::Mathf::Cos(i);
            float z = 5 * UnityEngine::Mathf::Sin(i);
            createCube(UnityEngine::Vector3(x, 0, z), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);
            cubeCount++;
        }

        for (int i = 0; i < 360; i += 5) {
            float x = 7 * UnityEngine::Mathf::Cos(i);
            float z = 7 * UnityEngine::Mathf::Sin(i);
            createCube(UnityEngine::Vector3(x, 0, z), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);
            cubeCount++;
        }

        for (int i = 0; i < 360; i += 5) {
            float x = 10 * UnityEngine::Mathf::Cos(i);
            float z = 10 * UnityEngine::Mathf::Sin(i);
            createCube(UnityEngine::Vector3(x, 0, z), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);
            cubeCount++;
        }

        for (int i = 0; i < 360; i += 5) {
            float x = 12 * UnityEngine::Mathf::Cos(i);
            float z = 12 * UnityEngine::Mathf::Sin(i);
            createCube(UnityEngine::Vector3(x, 0, z), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);
            cubeCount++;
        }

        for (int i = 0; i < 360; i += 5) {
            float x = 15 * UnityEngine::Mathf::Cos(i);
            float z = 15 * UnityEngine::Mathf::Sin(i);
            createCube(UnityEngine::Vector3(x, 0, z), UnityEngine::Vector3(0.1, 0.1, 0.1), pink);
            cubeCount++;
        }

        auto* menu = QuestUI::BeatSaberUI::CreateViewController<HMUI::ViewController*>();
        // lay the menu on the floor
        menu->get_transform()->set_position(UnityEngine::Vector3(0, -1, 0));

        auto* leftScreen = QuestUI::BeatSaberUI::CreateFloatingScreen({ 120, 240 }, { -15.0f, 17.0f, 25.0 }, { 0, 0, 0 }, 0.0f, false, false, 1);
        auto* rightScreen = QuestUI::BeatSaberUI::CreateFloatingScreen({ 120, 240 }, { 15.0f, 17.0f, 25.0 }, { 0, 0, 0 }, 0.0f, false, false, 1);
        leftScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(0);
        rightScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(0);
        HMUI::ImageView* leftImage = QuestUI::BeatSaberUI::CreateImage(leftScreen->get_transform(), QuestUI::BeatSaberUI::Base64ToSprite(banner2), { 0, 0 }, { 200.0f, 400.0f });
        HMUI::ImageView* rightImage = QuestUI::BeatSaberUI::CreateImage(rightScreen->get_transform(), QuestUI::BeatSaberUI::Base64ToSprite(banner2), { 0, 0 }, { 200.0f, 400.0f });

        auto* material = UnityEngine::Material::New_ctor(il2cpp_utils::newcsstr("PanoramaSkybox"));

        auto* texture = UnityEngine::Texture2D::New_ctor(100, 100);
        UnityEngine::ImageConversion::LoadImage(texture, IncludedAssets::background_jpg);
        texture->Apply();
        material->set_mainTexture(texture);

        // Create object
        backgroundObject = UnityEngine::GameObject::CreatePrimitive(UnityEngine::PrimitiveType::Sphere);
        backgroundObject->set_name(il2cpp_utils::newcsstr("_CustomBackground"));
        backgroundObject->set_layer(29);
        UnityEngine::Object::DontDestroyOnLoad(backgroundObject);

        // Material + shader management
        UnityEngine::Renderer* bgrenderer = backgroundObject->GetComponent<UnityEngine::Renderer*>();
        bgrenderer->set_sortingOrder(-8192);
        bgrenderer->set_material(material);
        backgroundMat = bgrenderer->get_material();
        backgroundMat->set_mainTexture(backgroundTexture);

        // Set transforms
        UnityEngine::Transform* bgtrans = backgroundObject->get_transform();
        bgtrans->set_localScale(UnityEngine::Vector3::get_one() * 100);
        bgtrans->set_localPosition(UnityEngine::Vector3::get_zero());
        bgtrans->set_localEulerAngles(UnityEngine::Vector3(0, 90, 180));


        auto* mainCamera = UnityEngine::Camera::get_main();
        mainCamera->set_backgroundColor(UnityEngine::Color(1.0f, 0.0f, 0.0f, 0.0f));
    }
}

// hook levelselectionnavigationcontroller to change the color of the lights
MAKE_HOOK_MATCH(LevelSelectionNavigationControllerHook, &GlobalNamespace::LevelSelectionNavigationController::DidActivate, void,
                GlobalNamespace::LevelSelectionNavigationController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    LevelSelectionNavigationControllerHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    auto* mainFlow = QuestUI::BeatSaberUI::GetMainFlowCoordinator();
    auto mfc = mainFlow->menuLightsManager;
    mfc->SetColorPreset(mainFlow->defaultLightsPreset, true);
}

// Hook the health warning screen to change the text
MAKE_HOOK_MATCH(HealthWarningUIHook, &GlobalNamespace::HealthWarningViewController::DidActivate, void,
                GlobalNamespace::HealthWarningViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    HealthWarningUIHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        self->healthAndSafetyTextMesh->set_text(getUwUQuote() + "\n\n Warning:\nIt's currently 2 am when i'm writing this. I'm so fucking drunk and tired and worked for some reason on this mod. Whatever you do with it, have fun. I've had mine making it.");
        self->healthAndSafetyTextMesh->set_fontSize(6.0f);
        self->duration = 20.0f;
        // use a custom font
    }
}

MAKE_HOOK_MATCH(AudioSource_Play, static_cast<void(UnityEngine::AudioSource::*)(double)>(&UnityEngine::AudioSource::Play), void, UnityEngine::AudioSource* self, double delay) {

    self->set_pitch(1);
    self->set_volume(5.0f);
    AudioSource_Play(self, delay);
}

MAKE_HOOK_MATCH(AudioSource_Play2, static_cast<void(UnityEngine::AudioSource::*)()>(&UnityEngine::AudioSource::Play), void, UnityEngine::AudioSource* self) {

    self->set_pitch(1);
    self->set_volume(5.0f);
    AudioSource_Play2(self);
}

MAKE_HOOK_MATCH(AudioSource_PlayScheduled, &UnityEngine::AudioSource::PlayScheduled, void, UnityEngine::AudioSource* self, double time) {

    self->set_pitch(1);
    self->set_volume(5.0f);
    AudioSource_PlayScheduled(self, time);
}
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/util.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"

#include "VRUIControls/VRPointer.hpp"

// hook the TextMeshProUGUI::set_text function to change the text
MAKE_HOOK_MATCH(TextMeshProUGUI_set_text, &HMUI::CurvedTextMeshPro::set_text, void, TMPro::TMP_Text* self, StringW value) {
    value = value + " UwU~";
    // unslant the text
    self->set_fontStyle(TMPro::FontStyles::Normal);
    // make it pink
    self->set_color(UnityEngine::Color(1.0f, 0.0f, 1.0f, 1.0f));
    TextMeshProUGUI_set_text(self, value);
}


/*MAKE_HOOK_MATCH(SkyboxHook, &GlobalNamespace::BloomPrePassBackgroundColor::InitIfNeeded, void, GlobalNamespace::BloomPrePassBackgroundColor* self) {
    SkyboxHook(self);

    UnityEngine::RenderSettings::set_fogColor(UnityEngine::Color(0.0f, 0.0f, 0.0f, 0.0f));

    // disable the skybox
    self->set_enabled(false);

    auto* material = UnityEngine::Material::New_ctor(il2cpp_utils::newcsstr("PanoramaSkybox"));
    material->set_shader(UnityEngine::Shader::Find(il2cpp_utils::newcsstr("Skybox/Panorama")));

    auto* texture = UnityEngine::Texture2D::New_ctor(100, 100);
    UnityEngine::ImageConversion::LoadImage(texture, IncludedAssets::background_jpg);
    material->set_mainTexture(texture);
    GlobalNamespace::BloomPrePassBackgroundColor::_set__material(material);
    self->color = UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);
}*/


// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), MainMenuUIHook);
    INSTALL_HOOK(getLogger(), HealthWarningUIHook);
    INSTALL_HOOK(getLogger(), AudioSource_Play);
    INSTALL_HOOK(getLogger(), AudioSource_Play2);
    INSTALL_HOOK(getLogger(), AudioSource_PlayScheduled);
    INSTALL_HOOK(getLogger(), TextMeshProUGUI_set_text);
    INSTALL_HOOK(getLogger(), LevelSelectionNavigationControllerHook);
    //INSTALL_HOOK(getLogger(), SkyboxHook);
    getLogger().info("Installed all hooks!");
}