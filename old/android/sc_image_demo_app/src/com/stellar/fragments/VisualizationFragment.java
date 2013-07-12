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
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Switch;
import android.widget.Toast;

import com.stellar.hpx.HPXViewPager;
import com.stellar.hpx.R;
import com.stellar.hpx.VisualizationRenderer;
import com.stellar.hpx.R.id;
import com.stellar.hpx.R.menu;
import com.stellar.views.VisualizationRelativeLayout;
import com.stellar.views.VisualizationView;

public class VisualizationFragment extends Fragment {
	private static final String TAG = "Visualization Fragment";
	public static final String ARG_SECTION_NUMBER = "0";
	
    /* HPX Related */
	private Runtime _runtime;
	private HPXViewPager _pager;
	
	private View view;
	private static VisualizationRelativeLayout _layout;

    public VisualizationFragment(Runtime runtime, HPXViewPager pager)
    {
        _runtime = runtime;
        _pager = pager;
    }
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
        Log.i(TAG, "onCreate");
	}
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
    	view = inflater.inflate(R.layout.visualization, container, false);
        // Inflate the layout for this fragment
    	_layout = (VisualizationRelativeLayout) view.findViewById(R.id.vis_rel_layout);
        _pager.setVisView((VisualizationView) view.findViewById(R.id.vis_view));

        return view;
    }
    
    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
    	super.onActivityCreated(savedInstanceState);
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    }
    
    public static VisualizationRelativeLayout getVisLayout() {
    	return _layout;
    }
}
