package com.bicubic.twice.LiveCloth;

import android.app.Activity;
import android.content.*;
import android.os.Bundle;
import android.view.*;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

public class MainActivity extends Activity {

    private static MainActivity context;
    private RelativeLayout mainLayout;
    private MainView mainView;

    @Override
    protected void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        MainActivity.context = this;

        mainLayout = new RelativeLayout(this);

        mainView = new MainView(this);
        mainLayout.addView(mainView, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        setContentView(mainLayout);
    }

    @Override
    protected void onPause() {
        mainView.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mainView.onResume();
    }

    public static MainActivity getContext() {
        return MainActivity.context;
    }

    public static void shareButtonPressed() {
        Intent intent = new Intent(android.content.Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
        intent.putExtra(Intent.EXTRA_SUBJECT, "Live Cloth demo project");
        intent.putExtra(Intent.EXTRA_TEXT, "https://github.com/puritanin/");
        getContext().startActivity(Intent.createChooser(intent, "How do you want to share?"));
    }
}
