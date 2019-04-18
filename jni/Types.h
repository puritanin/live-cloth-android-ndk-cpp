#ifndef _TYPES_H_
#define _TYPES_H_

// ----------------------------------------------------------------------------------------------------------------------------

#define POINTS_MAX_COUNT		4000
#define TOUCHES_MAX_COUNT       2

#define BUFFER_OFFSET(i)				((char *)NULL + (i))
#define RANDOM(range_min, range_max)	(float)(((double)rand() / (double)RAND_MAX) * (double)(range_max - range_min) + (double)range_min)
#define MAX(a,b)						(((a) > (b)) ? (a) : (b))

// ----------------------------------------------------------------------------------------------------------------------------

typedef struct {
	glm::vec4 vertex;
	glm::vec4 color;
} Particle;

typedef struct {
	glm::vec4 *aPoint;
	glm::vec4 position;
} PinConstraint;

typedef struct {
	glm::vec4 *aPoint, *bPoint;
	float distance;
	float stiffness;
} StiffnessConstraint;

typedef struct {
	Particle *targetPoint;
	int x;
	int y;

	float t;
	float direction;
	float beginValue;
	float endValue;
	float value;

	float localX;
	float localY;

	int keyIndex0;  //  0 * * 1
	int keyIndex1;  //  * * * *
	int keyIndex2;  //  * * * *
	int keyIndex3;  //  2 * * 3
} ColorPoint;

typedef struct {
    float x;
    float y;
} CGPoint;

typedef struct {
    float width;
    float height;
} CGSize;

typedef struct {
    union {
       CGPoint origin;
       struct {
           float x;
           float y;
       };
    };
    union {
       CGSize size;
       struct {
           float width;
           float height;
       };
    };
} CGRect;

typedef struct {
	glm::vec4 vertex;
	glm::vec2 texCoord;
} ButtonPoint;

typedef struct {
	ButtonPoint points[6];
} ButtonSprite;

typedef struct {
    bool isVisible;
    CGRect rect;
    glm::vec2 texCoord[4];
} ButtonInfo;

// ----------------------------------------------------------------------------------------------------------------------------

inline CGPoint CGPointMake(float x, float y)
{
    CGPoint p; p.x = x; p.y = y; return p;
}

inline CGSize CGSizeMake(float width, float height)
{
    CGSize s; s.width = width; s.height = height; return s;
}

inline CGRect CGRectMake(float x, float y, float width, float height)
{
    CGRect r; r.x = x; r.y = y; r.width = width; r.height = height; return r;
}

inline CGRect CGRectMake(CGPoint origin, CGSize size)
{
    CGRect r; r.origin = origin; r.size = size; return r;
}

inline bool CGRectContainsPoint(CGRect r, CGPoint p)
{
    return (p.x >= r.x) && (p.y >= r.y) && (p.x <= r.x + r.width) && (p.y <= r.y + r.height);
}

#endif // _TYPES_H_