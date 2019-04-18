package com.bicubic.twice.LiveCloth;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;


class MainView extends GLSurfaceView {

    private Renderer renderer;
    private TimerTask renderTimerTask;
	private Timer renderTimer;

    private final static int MAX_NUMBER_OF_POINTERS = 10;

    private int[] touchesIds = new int[MAX_NUMBER_OF_POINTERS];
    private float[] touchesParams = new float[MAX_NUMBER_OF_POINTERS * 4];
    private int[] allTouchesIds = new int[MAX_NUMBER_OF_POINTERS];

    private static Touch[] touches = new Touch[MAX_NUMBER_OF_POINTERS];

    static {
        for (int i = 0; i < MAX_NUMBER_OF_POINTERS; i++) {
            touches[i] = new Touch();
        }
    }


    public MainView(Context context) {
        super(context);

        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        renderer = new Renderer(context);
        setRenderer(renderer);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        renderTimerTask = new TimerTask() {
            @Override
            public void run() {
				requestRender();
            }
        };
    }

	private void stopRenderTimer() {
        if (renderTimer != null) {
			renderTimer.cancel();
			renderTimer = null;
		}
	}

	private void startRenderTimer() {
		stopRenderTimer();
		renderTimer = new Timer();
		renderTimer.schedule(renderTimerTask, 0, 17);
	}

    public void onPause() {
		stopRenderTimer();
        super.onPause();
    }

    public void onResume() {
        super.onResume();
        startRenderTimer();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int touchesCount = Math.min(e.getPointerCount(), MAX_NUMBER_OF_POINTERS);

        int actionMasked = e.getActionMasked();
        int actionIndex = e.getActionIndex();

        switch (actionMasked) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                int id = e.getPointerId(actionIndex);
                float x = e.getX(actionIndex);
                float y = e.getY(actionIndex);

                if (id < MAX_NUMBER_OF_POINTERS) {
                    touches[id].active = true;
                    touches[id].x = x;
                    touches[id].y = y;

                    touchesIds[0] = id;
                    touchesParams[0] = x;
                    touchesParams[1] = this.getHeight() - y;

                    for (int i = 0; i < touchesCount; i++) {
                        allTouchesIds[i] = e.getPointerId(i);
                    }

                    synchronized (renderer) {
                        NativeRenderer.touchesBegan(touchesIds, 1, touchesParams, allTouchesIds, touchesCount);
                    }
                }
                break;

            case MotionEvent.ACTION_MOVE:
                int touchesMovedCount = 0;

                for (int i = 0; i < touchesCount; i++) {
                    id = e.getPointerId(i);
                    x = e.getX(i);
                    y = e.getY(i);

                    if (id < MAX_NUMBER_OF_POINTERS) {
                        if (touches[id].active) {
                            if (Math.abs(touches[id].x - x) > 0.3f || Math.abs(touches[id].y - y) > 0.3f) {
                                touchesIds[touchesMovedCount] = id;
                                touchesParams[touchesMovedCount * 4] = x;
                                touchesParams[touchesMovedCount * 4 + 1] = this.getHeight() - y;
                                touchesParams[touchesMovedCount * 4 + 2] = touches[id].x;
                                touchesParams[touchesMovedCount * 4 + 3] = this.getHeight() - touches[id].y;
                                touchesMovedCount++;

                                touches[id].x = x;
                                touches[id].y = y;
                            }
                        }
                    }
                }

                if (touchesMovedCount > 0) {
                    synchronized (renderer) {
                        NativeRenderer.touchesMoved(touchesIds, touchesMovedCount, touchesParams);
                    }
                }
                break;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                id = e.getPointerId(actionIndex);

                if (id < MAX_NUMBER_OF_POINTERS) {
                    touches[id].active = false;
                    touchesIds[0] = id;

                    synchronized (renderer) {
                        NativeRenderer.touchesEnded(touchesIds, 1);
                    }
                }
                break;
        }
        return true;
    }


    private static class Renderer implements GLSurfaceView.Renderer {

        private Context context;
        private int[] textures = new int[1];
        private int width = 0, height = 0;
        private boolean isSurfaceCreated = false;

        public Renderer(Context context) {
            this.context = context;
        }

        public void onDrawFrame(GL10 gl) {
			NativeRenderer.render();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            float density = context.getResources().getDisplayMetrics().density;
            //System.out.println("renderer: onSurfaceChanged=" + width + ", " + height + " [x" + density + "]");

            if (this.width != width || this.height != height) {
                this.width = width;
                this.height = height;

                if (isSurfaceCreated) {
                    destroySurface();
                }

                createSurface(width, height, density);
            }
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            //System.out.println("renderer: onSurfaceCreated");
        }

        private void createSurface(int width, int height, float density) {
            // load texture atlas
            int buttonsImageId = R.raw.buttons_image_1;
            int buttonsMapId = R.raw.buttons_map_1;
            if (width > 450) {
                buttonsImageId = R.raw.buttons_image_2;
                buttonsMapId = R.raw.buttons_map_2;
            }
            if (width > 900) {
                buttonsImageId = R.raw.buttons_image_3;
                buttonsMapId = R.raw.buttons_map_3;
            }

            // image
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;
            Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), buttonsImageId, options);
            int texture = loadTextureFromBitmap(bitmap);
            int textureWidth = bitmap.getWidth();
            int textureHeight = bitmap.getHeight();
            bitmap.recycle();

            // map
            List<String> names = new ArrayList<String>();
            List<int[]> params = new ArrayList<int[]>();

            InputStream inputStream = context.getResources().openRawResource(buttonsMapId);
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String line;

            try {
                while ((line = bufferedReader.readLine()) != null) {
                    String[] keyValues = line.split("=");
                    String[] values = keyValues[1].trim().split(" ");
                    names.add(keyValues[0].trim());
                    params.add(new int[]{
                            Integer.parseInt(values[0]),
                            Integer.parseInt(values[1]),
                            Integer.parseInt(values[2]),
                            Integer.parseInt(values[3])});
                }
                inputStream.close();
            } catch (IOException e) {
            }

            NativeRenderer.setSpritesAtlas(texture, textureWidth, textureHeight, names.toArray(new String[names.size()]), params.toArray(new int[params.size()][]));
            NativeRenderer.create(width, height, density, ((float)width / density) > 599.0f ? 90 : 50);
            isSurfaceCreated = true;
        }

        private void destroySurface() {
            // dealloc everything
            NativeRenderer.destroy();
            GLES20.glDeleteTextures(1, textures, 0);
            isSurfaceCreated = false;
        }

        private int loadTextureFromBitmap(Bitmap bitmap) {
            GLES20.glGenTextures(1, textures, 0);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);

            GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

            ByteBuffer imageBuffer = ByteBuffer.allocateDirect(bitmap.getHeight() * bitmap.getWidth() * 4);
            imageBuffer.order(ByteOrder.nativeOrder());
            byte buffer[] = new byte[4];

            for (int i = 0; i < bitmap.getHeight(); i++) {
                for (int j = 0; j < bitmap.getWidth(); j++) {
                    int color = bitmap.getPixel(j, i);
                    buffer[0] = (byte) Color.red(color);
                    buffer[1] = (byte) Color.green(color);
                    buffer[2] = (byte) Color.blue(color);
                    buffer[3] = (byte) Color.alpha(color);
                    imageBuffer.put(buffer);
                }
            }

            imageBuffer.position(0);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, bitmap.getWidth(), bitmap.getHeight(), 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, imageBuffer);

            return textures[0];
        }

        protected void finalize() throws Throwable {
            try {
                //System.out.println("renderer: finalize");
                destroySurface();
            } finally {
                super.finalize();
            }
        }
    }
}
