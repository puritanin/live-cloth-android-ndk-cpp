#include "Buttons.h"

// ----------------------------------------------------------------------------------------------------------------------------

Buttons::Buttons()
{
}

Buttons::~Buttons()
{
}

void Buttons::Initialize(CGSize viewSize, float viewScale, CGSize textureSize, char** atlasNames, int** atlasParams, int atlasSize, float bannerHeight)
{
    _viewWidth = viewSize.width;
    _viewHeight = viewSize.height;
    _viewScale = viewScale;
    _textureSize = textureSize;

    _spriteAtlasSize = atlasSize;
    _spriteAtlasNames = atlasNames;
    _spriteAtlasParams = atlasParams;

    _spriteNames[BUTTON_SHARE] = "share-black";
    _spriteNames[BUTTON_ADS] = "ads-black";
    _spriteNames[BUTTON_PAUSE] = "pause-black";
    _spriteNames[BUTTON_RESUME] = "resume-black";
    _spriteNames[BUTTON_ROTATE] = "rotate-black";
    _spriteNames[BUTTON_SLIDER] = "slider-black";

    // create buttons

    CreateButtonWithIndex(BUTTON_SHARE, true);
    CreateButtonWithIndex(BUTTON_ADS, false);
    CreateButtonWithIndex(BUTTON_PAUSE, true);
    CreateButtonWithIndex(BUTTON_RESUME, false);
    CreateButtonWithIndex(BUTTON_ROTATE, true);
    CreateButtonWithIndex(BUTTON_SLIDER, false);


    float xOffset = _buttons[BUTTON_SHARE].rect.size.width / 8.0f;
    float yOffset = _viewHeight - xOffset;
    float yOffsetSlider = bannerHeight * _viewScale + xOffset * 2;

    _buttons[BUTTON_SHARE].rect.origin = CGPointMake(xOffset, yOffset - _buttons[BUTTON_SHARE].rect.size.height);
    _buttons[BUTTON_ADS].rect.origin = CGPointMake(xOffset + _buttons[BUTTON_SHARE].rect.size.width + xOffset, yOffset - _buttons[BUTTON_ADS].rect.size.height);

    _buttons[BUTTON_ROTATE].rect.origin = CGPointMake(_viewWidth - _buttons[BUTTON_ROTATE].rect.size.width - xOffset, yOffset - _buttons[BUTTON_ROTATE].rect.size.height);
    _buttons[BUTTON_SLIDER].rect.origin = CGPointMake((_viewWidth - _buttons[BUTTON_SLIDER].rect.size.width) / 2.0f, yOffsetSlider);

    // one place for two buttons
    _buttons[BUTTON_PAUSE].rect.origin = CGPointMake(_viewWidth - _buttons[BUTTON_ROTATE].rect.size.width - xOffset - _buttons[BUTTON_PAUSE].rect.size.width - xOffset, yOffset - _buttons[BUTTON_PAUSE].rect.size.height);
    _buttons[BUTTON_RESUME].rect.origin = _buttons[BUTTON_PAUSE].rect.origin;

    UpdateSprites();
}

bool Buttons::IsPressed(int touch, CGPoint point)
{
    if (_buttons[BUTTON_SHARE].isVisible && CGRectContainsPoint(_buttons[BUTTON_SHARE].rect, point)) {
        ShareButtonPressed();
        return true;
    }
    else if (_buttons[BUTTON_ADS].isVisible && CGRectContainsPoint(_buttons[BUTTON_ADS].rect, point)) {
        AdsButtonPressed();
        return true;
    }
    else if (_buttons[BUTTON_PAUSE].isVisible && CGRectContainsPoint(_buttons[BUTTON_PAUSE].rect, point)) {
        _buttons[BUTTON_PAUSE].isVisible = false;
        PauseButtonPressed();
        _buttons[BUTTON_RESUME].isVisible = true;
        UpdateSprites();
        return true;
    }
    else if (_buttons[BUTTON_RESUME].isVisible && CGRectContainsPoint(_buttons[BUTTON_RESUME].rect, point)) {
        _buttons[BUTTON_RESUME].isVisible = false;
        ResumeButtonPressed();
        _buttons[BUTTON_PAUSE].isVisible = true;
        UpdateSprites();
        return true;
    }
    else if (_buttons[BUTTON_ROTATE].isVisible && CGRectContainsPoint(_buttons[BUTTON_ROTATE].rect, point)) {
        _buttons[BUTTON_SLIDER].isVisible = !_buttons[BUTTON_SLIDER].isVisible;
        UpdateSprites();
        return true;
    }
    else if (_buttons[BUTTON_SLIDER].isVisible && CGRectContainsPoint(_buttons[BUTTON_SLIDER].rect, point)) {
        _touches[BUTTON_SLIDER] = touch;
        return true;
    }
    return false;
}

bool Buttons::IsSlided(int touch, CGPoint point, CGPoint previousPoint)
{
    if (touch == _touches[BUTTON_SLIDER]) {
        if (_buttons[BUTTON_SLIDER].isVisible && CGRectContainsPoint(_buttons[BUTTON_SLIDER].rect, point)) {
            float dX = previousPoint.x - point.x;
            RotateSlided(dX);
            return true;
        }
    }
    return false;
}

void Buttons::CreateButtonWithIndex(int index, bool isVisible)
{
    const char *spriteName = _spriteNames[index];
    int arrayIndex = SpriteNameToArrayIndex(spriteName);
    int *spriteRect = _spriteAtlasParams[arrayIndex];

    float x1 = spriteRect[0];
    float y1 = spriteRect[1];
    float width = spriteRect[2];
    float height = spriteRect[3];
    float x2 = x1 + width;
    float y2 = y1 + height;

    _buttons[index].isVisible = isVisible;
    _buttons[index].rect.size = CGSizeMake(width, height);

    _buttons[index].texCoord[0] = glm::vec2(x1 / _textureSize.width, y2 / _textureSize.height);
    _buttons[index].texCoord[1] = glm::vec2(x2 / _textureSize.width, y2 / _textureSize.height);
    _buttons[index].texCoord[2] = glm::vec2(x1 / _textureSize.width, y1 / _textureSize.height);
    _buttons[index].texCoord[3] = glm::vec2(x2 / _textureSize.width, y1 / _textureSize.height);
}

void Buttons::UpdateSprites()
{
    m_Count = 0;
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        if (_buttons[i].isVisible)
        {
            m_Sprites[m_Count].points[1].vertex = glm::vec4(_buttons[i].rect.origin.x, _buttons[i].rect.origin.y, 0.0f, 1.0f);
            m_Sprites[m_Count].points[2].vertex = glm::vec4(_buttons[i].rect.origin.x + _buttons[i].rect.size.width, _buttons[i].rect.origin.y, 0.0f, 1.0f);
            m_Sprites[m_Count].points[3].vertex = glm::vec4(_buttons[i].rect.origin.x, _buttons[i].rect.origin.y + _buttons[i].rect.size.height, 0.0f, 1.0f);
            m_Sprites[m_Count].points[4].vertex = glm::vec4(_buttons[i].rect.origin.x + _buttons[i].rect.size.width, _buttons[i].rect.origin.y + _buttons[i].rect.size.height, 0.0f, 1.0f);

            m_Sprites[m_Count].points[0].vertex = m_Sprites[m_Count].points[1].vertex;
            m_Sprites[m_Count].points[5].vertex = m_Sprites[m_Count].points[4].vertex;

            m_Sprites[m_Count].points[1].texCoord = _buttons[i].texCoord[0];
            m_Sprites[m_Count].points[2].texCoord = _buttons[i].texCoord[1];
            m_Sprites[m_Count].points[3].texCoord = _buttons[i].texCoord[2];
            m_Sprites[m_Count].points[4].texCoord = _buttons[i].texCoord[3];

            m_Count++;
        }
    }
}

void Buttons::ShowButtonWithIndex(int index, bool isVisible)
{
    _buttons[index].isVisible = isVisible;
    UpdateSprites();
}

int Buttons::SpriteNameToArrayIndex(const char *name)
{
    for (int i = 0; i < _spriteAtlasSize; i++) {
        if (strcmp(_spriteAtlasNames[i], name) == 0)
            return i;
    }
    return -1;
}
