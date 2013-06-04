package com.stellar.views;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.widget.RelativeLayout;

public class VisualizationRelativeLayout extends RelativeLayout {
	float mScaleFactor = 1;
	float mPivotX;
	float mPivotY;
	
	public VisualizationRelativeLayout(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	public VisualizationRelativeLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	public VisualizationRelativeLayout(Context context, AttributeSet attrs,
			int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
	}
	
	protected void dispatchDraw(Canvas canvas) {
		canvas.save(Canvas.MATRIX_SAVE_FLAG);
		canvas.scale(mScaleFactor, mScaleFactor, mPivotX, mPivotY);
		super.dispatchDraw(canvas);
		canvas.restore();
	}
	
	public void scale(float scaleFactor, float pivotX, float pivotY) {
		mScaleFactor = scaleFactor;
		mPivotX = pivotX;
		mPivotY = pivotY;
		this.invalidate();
	}
	
	public void restore() {
		mScaleFactor = 1;
		this.invalidate();
	}


}
