package com.bicubic.twice.LiveCloth;

public class NativeRenderer {

    static {
        System.loadLibrary("LiveCloth");
    }

    public static native void create(int width, int height, float density, int bannerHeight);

    public static native void render();

    public static native void destroy();

    public static native void setSpritesAtlas(int texture, int width, int height, String[] names, int[][] params);

    public static native void touchesBegan(int[] touchesIds, int touchesSize, float[] touchesParams, int[] allTouchesIds, int allTouchesSize);

    public static native void touchesMoved(int[] touchesIds, int touchesSize, float[] touchesParams);

    public static native void touchesEnded(int[] touchesIds, int touchesSize);
}
