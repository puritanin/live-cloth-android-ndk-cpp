
#include <jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <android/log.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

#include "Types.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Cloth.h"
#include "Shadow.h"
#include "ColorEffect.h"
#include "Buttons.h"


JNIEnv *m_Env;

int m_Width, m_Height;
float m_Density;
int m_BannerHeight;

bool m_IsPaused;

glm::mat4 m_Matrix2D;
int m_Texture;
CGSize m_TextureSize;
int m_AtlasSize;
char** m_AtlasNames;
int** m_AtlasParams;

ShaderProgram *m_ShaderProgram1, *m_ShaderProgram2;
Camera *m_Camera;
Cloth *m_Cloth;
Shadow *m_Shadow;
ColorEffect *m_ColorEffect;
Buttons *m_Buttons;

//-------------------------------------------------------------------------------------------------------------------------------

void ShareButtonPressed()
{
    jclass cls = m_Env->FindClass("com/bicubic/twice/LiveCloth/MainActivity");
    jmethodID method = m_Env->GetStaticMethodID(cls, "shareButtonPressed", "()V");
    m_Env->CallStaticVoidMethod(cls, method);
}

void AdsButtonPressed()
{
    //jclass cls = m_Env->FindClass("com/bicubic/twice/LiveCloth/MainActivity");
    //jmethodID method = m_Env->GetStaticMethodID(cls, "adsButtonPressed", "()V");
    //m_Env->CallStaticVoidMethod(cls, method);
}

void PauseButtonPressed()
{
	m_IsPaused = true;
}

void ResumeButtonPressed()
{
	m_IsPaused = false;
}

void RotateSlided(float offset)
{
    m_Camera->ChangeViewAngle(glm::radians(offset));
}

//-------------------------------------------------------------------------------------------------------------------------------

extern "C" {
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_create(JNIEnv *env, jobject obj,  jint width, jint height, jfloat density, jint bannerHeight);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_render(JNIEnv *env, jobject obj);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_destroy(JNIEnv *env, jobject obj);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_setSpritesAtlas(JNIEnv *env, jobject obj, jint texture, jint width, jint height, jobjectArray names, jobjectArray params);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesBegan(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize, jfloatArray touchesParams, jintArray allTouchesIds, jint allTouchesSize);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesMoved(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize, jfloatArray touchesParams);
    JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesEnded(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize);
};


JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_create(JNIEnv *env, jobject obj,  jint width, jint height, jfloat density, jint bannerHeight)
{
    //__android_log_print(ANDROID_LOG_INFO, "JNI", "NativeRenderer_create");
    m_Width = width;
    m_Height = height;
    m_Density = density;
    m_BannerHeight = bannerHeight;

    m_IsPaused = false;

    m_ShaderProgram1 = new ShaderProgram();
	m_ShaderProgram1->CreateAndAttachShaders(g_VertexShader1, g_FragmentShader1);
	m_ShaderProgram1->Link();
	m_ShaderProgram1->m_AttribLocations[ATTRIB_POSITION] = glGetAttribLocation(m_ShaderProgram1->m_Program, "a_Position");
	m_ShaderProgram1->m_AttribLocations[ATTRIB_COLOR] = glGetAttribLocation(m_ShaderProgram1->m_Program, "a_Color");
	m_ShaderProgram1->m_UniformLocations[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(m_ShaderProgram1->m_Program, "u_ModelViewProjectionMatrix");
	m_ShaderProgram1->m_UniformLocations[UNIFORM_POINT_SIZE] = glGetUniformLocation(m_ShaderProgram1->m_Program, "u_PointSize");

    m_ShaderProgram2 = new ShaderProgram();
	m_ShaderProgram2->CreateAndAttachShaders(g_VertexShader2, g_FragmentShader2);
	m_ShaderProgram2->Link();
	m_ShaderProgram2->m_AttribLocations[ATTRIB_POSITION] = glGetAttribLocation(m_ShaderProgram2->m_Program, "a_Position");
	m_ShaderProgram2->m_AttribLocations[ATTRIB_TEXCOORD] = glGetAttribLocation(m_ShaderProgram2->m_Program, "a_TexCoord");
	m_ShaderProgram2->m_UniformLocations[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(m_ShaderProgram2->m_Program, "u_ModelViewProjectionMatrix");
	m_ShaderProgram2->m_UniformLocations[UNIFORM_TEXTURE] = glGetUniformLocation(m_ShaderProgram2->m_Program, "u_Texture");

    glViewport(0, 0, m_Width, m_Height);

    m_Camera = new Camera();
	m_Camera->Initialize(glm::radians(-30.0f));
	m_Camera->SetProjection(glm::radians(65.0f), m_Width, m_Height, 0.1f, 10.0f);

    m_Matrix2D = glm::ortho(0.0f, (float)m_Width, 0.0f, (float)m_Height);

    m_Cloth = new Cloth();
	m_Cloth->Initialize(glm::vec4(0.0f, -0.001f, 0.0f, 0.0f), 0.99f);

    m_Shadow = new Shadow();
	m_Shadow->Initialize(m_Cloth->m_ClothParticles, m_Cloth->m_ClothParticlesCount);

    m_Cloth->m_PointSize = 2.0f;
    m_Cloth->m_LineWidth = 1.0f;
    m_Shadow->m_PointSize = 1.0f;
    m_Shadow->m_LineWidth = 1.0f;
    if (m_Width > 450) {
        m_Cloth->m_PointSize = 3.0f;
        m_Cloth->m_LineWidth = 2.0f;
        m_Shadow->m_PointSize = 2.0f;
        m_Shadow->m_LineWidth = 1.0f;
    }
    if (m_Width > 900) {
        m_Cloth->m_PointSize = 5.0f;
        m_Cloth->m_LineWidth = 4.0f;
        m_Shadow->m_PointSize = 3.0f;
        m_Shadow->m_LineWidth = 2.0f;
    }

    m_ColorEffect = new ColorEffect();
	m_ColorEffect->Initialize(m_Cloth->m_ClothParticles, CLOTH_SIZE_X, CLOTH_SIZE_Y, glm::vec4(1.0f, 0.4f, 0.2f, 1.0f));

    m_Buttons = new Buttons();
	m_Buttons->Initialize(CGSizeMake(m_Width, m_Height), m_Density, m_TextureSize, m_AtlasNames, m_AtlasParams, m_AtlasSize, m_BannerHeight);
	m_Buttons->ShareButtonPressed = &ShareButtonPressed;
	m_Buttons->AdsButtonPressed = &AdsButtonPressed;
	m_Buttons->PauseButtonPressed = &PauseButtonPressed;
	m_Buttons->ResumeButtonPressed = &ResumeButtonPressed;
	m_Buttons->RotateSlided = &RotateSlided;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_render(JNIEnv *env, jobject obj)
{
    if (!m_IsPaused) {
        m_Cloth->Update(0);
        m_Shadow->Update();
        m_ColorEffect->Update();
    }

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(m_ShaderProgram1->m_Program);

	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(m_ShaderProgram1->m_UniformLocations[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, (GLfloat *)&(m_Camera->m_ModelViewProjectionMatrix));

	// shadow
	glUniform1f(m_ShaderProgram1->m_UniformLocations[UNIFORM_POINT_SIZE], m_Shadow->m_PointSize);
	glLineWidth(m_Shadow->m_LineWidth);

	glEnableVertexAttribArray(m_ShaderProgram1->m_AttribLocations[ATTRIB_POSITION]);
	glVertexAttribPointer(m_ShaderProgram1->m_AttribLocations[ATTRIB_POSITION], 4, GL_FLOAT, GL_FALSE, sizeof(Particle), &m_Shadow->m_ShadowPoints[0].vertex);
	glEnableVertexAttribArray(m_ShaderProgram1->m_AttribLocations[ATTRIB_COLOR]);
	glVertexAttribPointer(m_ShaderProgram1->m_AttribLocations[ATTRIB_COLOR], 4, GL_FLOAT, GL_FALSE, sizeof(Particle), &m_Shadow->m_ShadowPoints[0].color);

	glDrawArrays(GL_POINTS, 0, m_Cloth->m_ClothParticlesCount);
	glDrawElements(GL_LINES, m_Cloth->m_ClothLinesCount, GL_UNSIGNED_SHORT, m_Cloth->m_ClothLines);

	// cloth
	glUniform1f(m_ShaderProgram1->m_UniformLocations[UNIFORM_POINT_SIZE], m_Cloth->m_PointSize);
	glLineWidth(m_Cloth->m_LineWidth);

	glEnableVertexAttribArray(m_ShaderProgram1->m_AttribLocations[ATTRIB_POSITION]);
	glVertexAttribPointer(m_ShaderProgram1->m_AttribLocations[ATTRIB_POSITION], 4, GL_FLOAT, GL_FALSE, sizeof(Particle), &m_Cloth->m_ClothParticles[0].vertex);
	glEnableVertexAttribArray(m_ShaderProgram1->m_AttribLocations[ATTRIB_COLOR]);
	glVertexAttribPointer(m_ShaderProgram1->m_AttribLocations[ATTRIB_COLOR], 4, GL_FLOAT, GL_FALSE, sizeof(Particle), &m_Cloth->m_ClothParticles[0].color);

	glDrawArrays(GL_POINTS, 0, m_Cloth->m_ClothParticlesCount);
	glDrawElements(GL_LINES, m_Cloth->m_ClothLinesCount, GL_UNSIGNED_SHORT, m_Cloth->m_ClothLines);

	glDisable(GL_DEPTH_TEST);

	// 2D
    glUseProgram(m_ShaderProgram2->m_Program);

    glUniformMatrix4fv(m_ShaderProgram2->m_UniformLocations[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, (GLfloat *)&m_Matrix2D);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glUniform1i(m_ShaderProgram2->m_UniformLocations[UNIFORM_TEXTURE], 0);

    glEnableVertexAttribArray(m_ShaderProgram2->m_AttribLocations[ATTRIB_POSITION]);
    glVertexAttribPointer(m_ShaderProgram2->m_AttribLocations[ATTRIB_POSITION], 4, GL_FLOAT, GL_FALSE, sizeof(ButtonPoint), &m_Buttons->m_Sprites[0].points[0].vertex);
    glEnableVertexAttribArray(m_ShaderProgram2->m_AttribLocations[ATTRIB_TEXCOORD]);
    glVertexAttribPointer(m_ShaderProgram2->m_AttribLocations[ATTRIB_TEXCOORD], 2, GL_FLOAT, GL_FALSE, sizeof(ButtonPoint), &m_Buttons->m_Sprites[0].points[0].texCoord);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_Buttons->m_Count * 6);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glUseProgram(0);
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_destroy(JNIEnv *env, jobject obj)
{
    //__android_log_print(ANDROID_LOG_INFO, "JNI", "NativeRenderer_destroy");
    for (int i = 0; i < m_AtlasSize; i++) {
        delete m_AtlasNames[i];
        delete m_AtlasParams[i];
    }
    delete m_AtlasNames;
    delete m_AtlasParams;

    delete m_Buttons;
    delete m_ColorEffect;
    delete m_Shadow;
    delete m_Cloth;
    delete m_Camera;
    delete m_ShaderProgram2;
    delete m_ShaderProgram1;
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_setSpritesAtlas(JNIEnv *env, jobject obj, jint texture, jint width, jint height, jobjectArray names, jobjectArray params)
{
    //__android_log_print(ANDROID_LOG_INFO, "JNI", "NativeRenderer_setSpritesAtlas");
    m_Texture = texture;
    m_TextureSize = CGSizeMake(width, height);

    m_AtlasSize = env->GetArrayLength(names);
    m_AtlasNames = new char*[m_AtlasSize];
    m_AtlasParams = new int*[m_AtlasSize];

    for (int i = 0; i < m_AtlasSize; i++) {
        jstring string = (jstring)env->GetObjectArrayElement(names, i);
        const char *rawString = env->GetStringUTFChars(string, 0);
        m_AtlasNames[i] = new char[strlen(rawString) + 1];
        strcpy(m_AtlasNames[i], rawString);
        env->ReleaseStringUTFChars(string, rawString);

        jintArray intArray = (jintArray)env->GetObjectArrayElement(params, i);
        int intArraySize = env->GetArrayLength(intArray);
        m_AtlasParams[i] = new int[intArraySize];

        jint *intValues = env->GetIntArrayElements(intArray, 0);
        for (int j = 0; j < intArraySize; j++) {
            m_AtlasParams[i][j] = intValues[j];
        }
        env->ReleaseIntArrayElements(intArray, intValues, 0);
    }

    /*for (int i = 0; i < m_AtlasSize; i++) {
        __android_log_print(ANDROID_LOG_INFO, "JNI", "%i:%s", i, m_AtlasNames[i]);
        for (int j = 0; j < 4; j++) {
            __android_log_print(ANDROID_LOG_INFO, "JNI", "  %i:%i", j, m_AtlasParams[i][j]);
        }
    }*/
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesBegan(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize, jfloatArray touchesParams, jintArray allTouchesIds, jint allTouchesSize)
{
    m_Env = env;

    jint* ids = env->GetIntArrayElements(touchesIds, 0);
    jfloat* params = env->GetFloatArrayElements(touchesParams, 0);
    jint* allIds = env->GetIntArrayElements(allTouchesIds, 0);

    int emptySlotsCount = 0;
    int emptySlots[TOUCHES_MAX_COUNT];

    // checking for dead touches and empty slots
    for (int i = 0; i < TOUCHES_MAX_COUNT; i++)
    {
        int j;
        for (j = 0; j < allTouchesSize; j++) {
            if (allIds[j] == m_Cloth->m_Touches[i]) break;
        }

        // not found
        if (j == allTouchesSize) {
            m_Cloth->m_Touches[i] = -1;

            emptySlots[emptySlotsCount] = i;
            emptySlotsCount++;
        }
    }

    // processing each new touch
    for (int i = 0; i < touchesSize; i++)
    {
        int id = ids[i];
        float x = params[i * 4];
        float y = params[i * 4 + 1];

        if (m_Buttons->IsPressed(id, CGPointMake(x, y))) continue;

        if (emptySlotsCount > 0 && !m_IsPaused)
        {
            glm::vec3 nearPoint = glm::unProject(glm::vec3(x, y, 0.0f), m_Camera->m_ViewMatrix, m_Camera->m_ProjectionMatrix, glm::vec4(0, 0, m_Width, m_Height));
            glm::vec3 farPoint = glm::unProject(glm::vec3(x, y, 1.0f), m_Camera->m_ViewMatrix, m_Camera->m_ProjectionMatrix, glm::vec4(0, 0, m_Width, m_Height));

            // search nearest point
            int nearestIndex = m_Cloth->FindNearestIndex(nearPoint, farPoint);
            if (nearestIndex != -1)
            {
                emptySlotsCount--;
                int index = emptySlots[emptySlotsCount];

                m_Cloth->m_Touches[index] = id;
                m_Cloth->m_TouchPoints[index] = &m_Cloth->m_ClothParticles[nearestIndex].vertex;
                m_Cloth->UpdateTouchPosition(index, nearPoint, farPoint);

                m_ColorEffect->SetMode2(true, index, nearestIndex % CLOTH_SIZE_X, nearestIndex / CLOTH_SIZE_X);
            }
        }
    }

    env->ReleaseIntArrayElements(touchesIds, ids, 0);
    env->ReleaseFloatArrayElements(touchesParams, params, 0);
    env->ReleaseIntArrayElements(allTouchesIds, allIds, 0);
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesMoved(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize, jfloatArray touchesParams)
{
    jint* ids = env->GetIntArrayElements(touchesIds, 0);
    jfloat* params = env->GetFloatArrayElements(touchesParams, 0);

    for (int i = 0; i < touchesSize; i++)
    {
        int id = ids[i];
        float x = params[i * 4];
        float y = params[i * 4 + 1];
        float x0 = params[i * 4 + 2];
        float y0 = params[i * 4 + 3];

        if (m_Buttons->IsSlided(id, CGPointMake(x, y), CGPointMake(x0, y0))) continue;

        if (!m_IsPaused)
        {
			for (int i = 0; i < TOUCHES_MAX_COUNT; i++)
			{
				if (m_Cloth->m_Touches[i] == id)
				{
                    glm::vec3 nearPoint = glm::unProject(glm::vec3(x, y, 0.0f), m_Camera->m_ViewMatrix, m_Camera->m_ProjectionMatrix, glm::vec4(0, 0, m_Width, m_Height));
                    glm::vec3 farPoint = glm::unProject(glm::vec3(x, y, 1.0f), m_Camera->m_ViewMatrix, m_Camera->m_ProjectionMatrix, glm::vec4(0, 0, m_Width, m_Height));

					m_Cloth->UpdateTouchPosition(i, nearPoint, farPoint);
				}
			}
        }
    }

    env->ReleaseIntArrayElements(touchesIds, ids, 0);
    env->ReleaseFloatArrayElements(touchesParams, params, 0);
}

JNIEXPORT void JNICALL Java_com_bicubic_twice_LiveCloth_NativeRenderer_touchesEnded(JNIEnv *env, jobject obj, jintArray touchesIds, jint touchesSize)
{
    jint* ids = env->GetIntArrayElements(touchesIds, 0);

	int emptySlotsCount = 0;
    for (int i = 0; i < TOUCHES_MAX_COUNT; i++)
    {
		// reset ended or cancelled touch
        int j;
        for (j = 0; j < touchesSize; j++) {
            if (ids[j] == m_Cloth->m_Touches[i]) break;
        }

        if (j != touchesSize) {
            m_Cloth->m_Touches[i] = -1;
            m_ColorEffect->SetMode2(false, i, 0, 0);
        }

        if (m_Cloth->m_Touches[i] == -1) {
            emptySlotsCount++;
        }
    }

	// reset color effect, if all slots are empty
    if (emptySlotsCount == TOUCHES_MAX_COUNT) {
        m_ColorEffect->SetMode1();
    }

    env->ReleaseIntArrayElements(touchesIds, ids, 0);
}
