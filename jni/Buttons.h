#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <android/log.h>
#include <string.h>

#include "Types.h"

// ----------------------------------------------------------------------------------------------------------------------------

enum {
	BUTTON_SHARE,
	BUTTON_ADS,
	BUTTON_PAUSE,
	BUTTON_RESUME,
    BUTTON_ROTATE,
    BUTTON_SLIDER,
	NUM_BUTTONS
};

// buttons delegate
typedef void (*ShareButtonPressedT) ();
typedef void (*AdsButtonPressedT) ();
typedef void (*PauseButtonPressedT) ();
typedef void (*ResumeButtonPressedT) ();
typedef void (*RotateSlidedT) (float offset);

class Buttons
{
public:
	Buttons();
	~Buttons();

	void Initialize(CGSize viewSize, float viewScale, CGSize textureSize, char** atlasNames, int** atlasParams, int atlasSize, float bannerHeight);
    bool IsPressed(int touch, CGPoint point);
    bool IsSlided(int touch, CGPoint point, CGPoint previousPoint);
    void CreateButtonWithIndex(int index, bool isVisible);
    void UpdateSprites();
    void ShowButtonWithIndex(int index, bool isVisible);

    ShareButtonPressedT ShareButtonPressed;
    AdsButtonPressedT AdsButtonPressed;
    PauseButtonPressedT PauseButtonPressed;
    ResumeButtonPressedT ResumeButtonPressed;
    RotateSlidedT RotateSlided;

    int m_Count;
    ButtonSprite m_Sprites[NUM_BUTTONS];

private:
    int SpriteNameToArrayIndex(const char *name);

    const char* _spriteNames[NUM_BUTTONS];

    int _spriteAtlasSize;
    char** _spriteAtlasNames;
    int** _spriteAtlasParams;

    ButtonInfo _buttons[NUM_BUTTONS];
    int _touches[NUM_BUTTONS];

    CGSize _textureSize;

    float _viewWidth;
    float _viewHeight;
    float _viewScale;
};

#endif // _BUTTONS_H_