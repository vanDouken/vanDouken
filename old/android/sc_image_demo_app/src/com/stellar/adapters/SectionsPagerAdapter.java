package com.stellar.adapters;

import hpx.android.Runtime;
import hpx.android.perfcounters.PerfCounterFragment;
import android.content.Context;
import android.os.Bundle;
import android.app.ActionBar;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.app.FragmentPagerAdapter;
import android.util.Log;

import com.stellar.fragments.VisualizationFragment;
import com.stellar.fragments.CameraFragment;
import com.stellar.hpx.HPXViewPager;
import com.stellar.hpx.R;




public class SectionsPagerAdapter extends FragmentPagerAdapter {
	private static String TAG = "SectionsPagerAdapter";
	private Context _context;

    Runtime _runtime;
    HPXViewPager pager;

	public SectionsPagerAdapter(FragmentManager fm, Context context, Runtime runtime, HPXViewPager pager) {
		super(fm);
		_context = context;
        _runtime = runtime;
        this.pager = pager;
	}

	@Override
	public Fragment getItem(int i) {
		Bundle args;
		switch(i) {
		case 0:
			VisualizationFragment visualizationFragment = new VisualizationFragment(_runtime, pager);
			args = new Bundle();
			args.putInt(VisualizationFragment.ARG_SECTION_NUMBER, i + 1);
			visualizationFragment.setArguments(args);
			return visualizationFragment;
		case 1:
			CameraFragment cameraFragment = new CameraFragment(_runtime, pager);
			args = new Bundle();
			args.putInt(CameraFragment.ARG_SECTION_NUMBER, i + 1);
			cameraFragment.setArguments(args);
			return cameraFragment;
        /*
		case 2:
			PerfCounterFragment counterFragment = new PerfCounterFragment(_runtime);
			args = new Bundle();
			args.putInt(PerfCounterFragment.ARG_SECTION_NUMBER, i+1);
			counterFragment.setArguments(args);
			return counterFragment;
        */
		default:
			Log.wtf(TAG, "Not supposed to be here");
			return null;
		}
	}

	@Override
	public int getCount() {
		return 2;
	}
	
    @Override
    public CharSequence getPageTitle(int position) {
        switch (position) {
            case 0: return _context.getString(R.string.title_section0).toUpperCase();
            case 1: return _context.getString(R.string.title_section1).toUpperCase();
            //case 2: return _context.getString(R.string.title_section2).toUpperCase();
        }
        return null;
    }
}
