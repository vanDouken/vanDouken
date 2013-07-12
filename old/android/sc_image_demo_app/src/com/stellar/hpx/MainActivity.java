package com.stellar.hpx;

import hpx.android.Runtime;

import java.io.FileWriter;
import java.io.IOException;

import android.app.ActionBar;
import android.content.res.Configuration;
import android.content.pm.ActivityInfo;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;

import android.view.WindowManager;
import android.view.Window;

import com.stellar.adapters.SectionsPagerAdapter;

public class MainActivity extends FragmentActivity {

	private HPXViewPager _pager;
	private SectionsPagerAdapter _pagerAdapter;
	private ActionBar bar;
	
    Runtime runtime;
    WifiLock wifiLock;

    static
    {
        Runtime.loadLibraries();
        System.loadLibrary("hpiif_android");
    }

    private void copyInit()
    {
        try {
            FileWriter out = new FileWriter(getFilesDir().getPath().toString() + "/flowingcanvas_client.ini");
            String content =
                "[hpx.components.flowingcanvas_updategroup]\n"
              + "name! = hpiif_android\n"
              + "path! = lib/libhpiif_android.so\n"
              + "enabled! = 0\n"
              + "\n"
              + "[hpx.components.flowingcanvas_simulator]\n"
              + "name! = hpiif_android\n"
              + "path! = lib/libhpiif_android.so\n"
              + "enabled! = 0\n"
              + "\n"
              + "[hpx.components.flowingcanvas_gui]\n"
              + "name! = hpiif_android\n"
              + "path! = lib/libhpiif_android.so\n"
              + "enabled! = 1\n"
              + "[hpx.threadpools]\n"
              + "io_pool_size = 1\n"
              + "parcel_pool_size = 1\n"
              + "timer_pool_size = 1\n"
              + "[hpx.parcel]\n"
              + "max_connections_per_locality = 2\n"
                ;

            out.write(content, 0, content.length());
            out.close();
        }
        catch (IOException e)
        {
            throw new RuntimeException(e);
        }
    }

    boolean running = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        if(!running)
        {
            WifiManager wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE);
            wifiLock = wifiManager.createWifiLock(WifiManager.WIFI_MODE_FULL_HIGH_PERF, "hpx.android");
            wifiLock.acquire();
            runtime = new Runtime();
            copyInit();

            int ip = wifiManager.getConnectionInfo().getIpAddress();

            String[] args = {
                "--hpx:threads=2"
              , "--hpx:config=" + getFilesDir().getPath().toString() + "/flowingcanvas_client.ini"
              , "--hpx:run-hpx-main"
              //, "--hpx:connect"
              , "--hpx:hpx=" + String.format("%d.%d.%d.%d", (ip & 0xff), (ip >> 8 & 0xff),  (ip >> 16 & 0xff),  (ip >> 24 & 0xff))
              , "--standalone"
              //, "--hpx:agas=192.168.0.1"
              //, "-Ihpx.logging.level=5"
            };
            runtime.init(args);
        
        setContentView(R.layout.activity_main);
        
        Context context = this;

            // Set up the ViewPager with the sections adapter.
            _pager = (HPXViewPager) findViewById(R.id.pager);
            _pagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager(), context, runtime, _pager);
            
            _pager.setAdapter(_pagerAdapter);
            _pager.setCurrentItem(0, true);
            
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            running = true;
        }
        
        //bar = getActionBar();
        //bar.hide();
        /*
        bar.setDisplayShowHomeEnabled(false);
        bar.setDisplayShowTitleEnabled(false);
        */
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        /*
        wifiLock.release();
        runtime.stop();
        runtime = null;
        */
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);
        /*
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        */
    }
}
