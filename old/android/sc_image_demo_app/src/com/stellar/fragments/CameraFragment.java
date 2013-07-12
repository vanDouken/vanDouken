package com.stellar.fragments;

import hpx.android.Runtime;
import hpx.android.ssh.InvokeSSH;
import android.app.ActionBar;
import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Switch;
import android.widget.Toast;

import android.graphics.PointF;

import android.app.AlertDialog;
import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.MotionEvent;

import java.io.IOException;
import java.util.List;

import com.stellar.hpx.HPXViewPager;
import com.stellar.hpx.R;

public class CameraFragment extends Fragment {
	private static final String TAG = "Camera Fragment";
	public static final String ARG_SECTION_NUMBER = "1";
	
    private Runtime _runtime;
	private HPXViewPager _pager;

    private PointF lastPoint;

    private Preview preview;
    Camera camera;
    int numberOfCameras;

    int cameraId;

    boolean first = true;
    boolean takingPicture = false;
	
    public CameraFragment(Runtime runtime, HPXViewPager pager)
    {
        _runtime = runtime;
        _pager = pager;
    }
	
    @Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate");

        preview = new Preview(this.getActivity(), this);

        numberOfCameras = Camera.getNumberOfCameras();
		
        CameraInfo cameraInfo = new CameraInfo();
        for(int i = 0; i < numberOfCameras; i++)
        {
            Camera.getCameraInfo(i, cameraInfo);
            if(cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT)
            {
                cameraId = i;
            }
        }

        Log.i(TAG, "default camera: " + cameraId);

	}
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return preview;
    }
    
    @Override
    public void onResume() {
        super.onResume();
        Log.i(TAG, "onResume");

        if(first)
        {
            openCamera(cameraId);
            preview.setCamera(camera);
            first = false;
            return;
        }
        
        if(camera != null)
        {
            camera.stopPreview();
            preview.setCamera(null);
            camera.release();
        }

        // Open the default i.e. the first rear facing camera.
        openCamera(cameraId);
        preview.switchCamera(camera);
        camera.startPreview();
    }
    
    @Override
    public void onPause() {
    	super.onPause();

        // Because the Camera object is a shared resource, it's very
        // important to release it when the activity is paused.
        releaseCameraAndPreview();
        if(camera != null)
        {
            camera.stopPreview();
            preview.switchCamera(null);
            camera.release();
            camera = null;
        }
    }

    private void openCamera(int id)
    {
        try {
            releaseCameraAndPreview();
            camera = Camera.open(id);
            Camera.Parameters parameters = camera.getParameters();
            List<Camera.Size> sizes = parameters.getSupportedPictureSizes();
            for(int i = 0; i < sizes.size(); ++i)
            {
                Log.i(TAG, "Picture sizes: " + sizes.get(i).width + " " + sizes.get(i).height);
            }
            parameters.setPictureSize(640, 480);
            camera.setParameters(parameters);
            cameraId = id;
            Log.i(TAG, "opened camera " + cameraId);
        }
        catch (Exception e) 
        {
            Log.e(TAG, "failed to open camera");
            e.printStackTrace();
        }
    }

    private void releaseCameraAndPreview()
    {
        //preview.setCamera(null);
        /*
        if(camera != null)
        {
            camera.release();
            camera = null;
        }
        */
    }
    
    public void onTouchEvent(MotionEvent e) {

    	switch(e.getAction()) {
    	case MotionEvent.ACTION_UP:
            {
                if(numberOfCameras == 1)
                {
                    break;
                }
                //Log.i(TAG, "Action UP");
                PointF tmp = new PointF(e.getX(), e.getY());
                PointF vec = new PointF(tmp.x - lastPoint.x, tmp.y - lastPoint.y);
                double distance = java.lang.Math.sqrt(vec.x * vec.x + vec.y * vec.y);
                //Log.i(TAG, "Distance " + distance);
                if(distance > 10.0)
                {
                    if(camera != null)
                    {
                        camera.stopPreview();
                        preview.setCamera(null);
                        camera.release();
                    }

                    openCamera((cameraId + 1) % numberOfCameras);
                    preview.switchCamera(camera);
                    camera.startPreview();
                    Log.i(TAG, "Switch camera?");
                }
                else
                {
                    if(takingPicture) return;
                    takingPicture = true;
                    Log.i(TAG, "Take Picture?");
                    camera.takePicture(
                        // Shutter callback
                        new Camera.ShutterCallback()
                        {
                            public void onShutter()
                            {
                            }
                        }
                        // Raw Callback
                      , new Camera.PictureCallback()
                        {
                            public void onPictureTaken(byte[] data, Camera camera)
                            {
                            }
                        }
                        // Postview Callback
                      , new Camera.PictureCallback()
                        {
                            public void onPictureTaken(byte[] data, Camera camera)
                            {
                            }
                        }
                        // jpeg Callback
                      , new Camera.PictureCallback()
                        {
                            public void onPictureTaken(byte[] data, Camera camera)
                            {
                                Camera.Parameters parameters = camera.getParameters();
                                Camera.Size size = parameters.getPictureSize();
                                Log.i(TAG, size.width + " " + size.height);
                                _runtime.apply("new_image", data, size.width, size.height);
                                camera.startPreview();
                                takingPicture = false;
                                _pager.setCurrentItem(0);
                            }
                        }
                    );
                }
                lastPoint= null;
                //Log.i(TAG, lastPoint.x + " " + lastPoint.y);
            }
    		break;
    	case MotionEvent.ACTION_DOWN:
            lastPoint = new PointF(e.getX(), e.getY());
    		break;
    	default:
    		break;
    	}
    }
}

