#define DECLARE_FILE(name, prefix) extern "C" uint8_t _binary_##name##_start[]; extern "C" uint8_t _binary_##name##_end[]; struct prefix##name { static size_t getLength() { return _binary_##name##_end - _binary_##name##_start; } static uint8_t* getData() { return _binary_##name##_start; } };
DECLARE_FILE(banner_base64,)
DECLARE_FILE(banner_png,)
DECLARE_FILE(campaign_image_base64,)
DECLARE_FILE(campaign_image_png,)
DECLARE_FILE(group_image_base64,)
DECLARE_FILE(group_image_png,)
DECLARE_FILE(multi_image_base64,)
DECLARE_FILE(multi_image_png,)
DECLARE_FILE(solo_image_base64,)
DECLARE_FILE(solo_image_png,)
DECLARE_FILE(uwu_base64,)
DECLARE_FILE(uwu_mp3,)
DECLARE_FILE(uwu_wav,)