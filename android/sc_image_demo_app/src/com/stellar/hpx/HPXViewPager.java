package com.stellar.hpx;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;

import com.stellar.views.VisualizationView;

public class HPXViewPager extends ViewPager {

	private boolean isEnabled;
	
	public HPXViewPager(Context context) {
		super(context);
		this.isEnabled = true;
		
	}
	
	public HPXViewPager(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.isEnabled = true;
	}

    VisualizationView visView = null;

    public void setVisView(VisualizationView v)
    {
        visView = v;
    }

	
	@Override
	public boolean onTouchEvent(MotionEvent event) {
        //Log.i("...", getWidth() + " " + getHeight());
        int w = getWidth();
        int h = getHeight();
        float x = event.getX();
        float y = event.getY();
        //Log.i("...", x + " " + y);
        int currentItem = getCurrentItem();
    	switch(event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                if(visView != null)
                {
                    if(currentItem == 0)
                    {
                        visView.stopRender();
                    }
                    else
                    {
                        visView.startRender();
                    }
                }
                if(currentItem > 0 && y < 50 && x < w * 0.25)
                {
                    setCurrentItem(currentItem - 1);
                    break;
                }
                if(currentItem < 2 && y < 50 && x > w - (w * 0.25))
                {
                    setCurrentItem(currentItem + 1);
                    break;
                }
            default:
                break;
        }

		return false;
	}

	@Override
	public boolean onInterceptTouchEvent(MotionEvent event) {
        /*
        Log.i("intercept", getWidth() + " " + getHeight());
        int w = getWidth();
        int h = getHeight();
        float x = event.getX();
        float y = event.getY();
        int currentItem = getCurrentItem();
    	switch(event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                if(currentItem > 0 && y < 50 && x < 100)
                {
                    setCurrentItem(currentItem - 1);
                    break;
                }
                if(currentItem < 2 && y < 50 && x > w - 100)
                {
                    setCurrentItem(currentItem + 1);
                    break;
                }
            default:
                break;
        }
        */
	 
	    return false;
	}
	 
	public void setPagingEnabled(boolean enabled) {
		this.isEnabled = enabled;
	}
	
}
