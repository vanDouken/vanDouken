
//  Copyright (c) 2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

package com.stellar.views;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

import com.stellar.fragments.VisualizationFragment;
import com.stellar.hpx.VisualizationRenderer;

public class VisualizationView extends GLSurfaceView {
    private final VisualizationRenderer renderer;
	private ScaleGestureDetector _scaleDetector;
	private VisualizationPinchListener _pinchListen;
	
    private Timer timer;

    public VisualizationView(Context context, AttributeSet unused) {
        super(context);

        Log.i("VisualizationView", "constructing ...");

        setEGLContextClientVersion(2);
        renderer = new VisualizationRenderer(this, context);
        _scaleDetector = new ScaleGestureDetector(context, new VisualizationPinchListener());
        setRenderer(renderer);
        //setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        //setDebugFlags(GLSurfaceView.DEBUG_CHECK_GL_ERROR);
        timer = new Timer();

        startRender();
    }

    public void startRender()
    {
        Log.i("VisualizationView", "start");
        timer.schedule(
            new TimerTask()
            {
                public void run()
                {
                    requestRender();
                }
            }, 0, 500
        );
    }
    public void stopRender()
    {
        Log.i("VisualizationView", "stop");
        timer.cancel();
    }

    @Override
    public void onFocusChanged(boolean b, int i, android.graphics.Rect rect)
    {
        Log.i("!!!", "focus changed");
        super.onFocusChanged(b, i, rect);
    }
    
    @Override
    public void onWindowFocusChanged(boolean b)
    {
        Log.i("!!!", "windows focus changed");
        super.onWindowFocusChanged(b);
    }
    
    @Override
    public void onAttachedToWindow()
    {
        Log.i("!!!", "attach");
        super.onAttachedToWindow();
    }
    
    @Override
    public void onDetachedFromWindow()
    {
        Log.i("!!!", "deattach");
        super.onDetachedFromWindow();
    }
    
    @Override
    public void onVisibilityChanged(android.view.View v, int i)
    {
        Log.i("!!!", "vis change");
        super.onVisibilityChanged(v, i);
    }
    
    @Override
    public void onWindowVisibilityChanged(int i)
    {
        Log.i("!!!", "window vis change");
        super.onWindowVisibilityChanged(i);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
    	
    	//_scaleDetector.onTouchEvent(e);
    	renderer.onTouchEvent(e);
    	

        return true;
    }
    
    class VisualizationPinchListener extends SimpleOnScaleGestureListener {
    	float startingSpan;
    	float endSpan;
    	float startFocusX;
    	float startFocusY;
    	

    	public boolean onScaleBegin(ScaleGestureDetector detector) {
    		startingSpan = detector.getCurrentSpan();
    		startFocusX = detector.getFocusX();
    		startFocusY = detector.getFocusY();
    		return true;
    	}
    	
    	public boolean onScale(ScaleGestureDetector detector) {
    		VisualizationFragment.getVisLayout().scale(detector.getCurrentSpan()/startingSpan, startFocusX, startFocusY);
    		return true;
    	}
    	
    	public void onScaleEnd(ScaleGestureDetector detector) {
    		VisualizationFragment.getVisLayout().restore();
    	}
    }
}



