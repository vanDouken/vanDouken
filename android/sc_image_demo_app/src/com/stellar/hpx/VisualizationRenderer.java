package com.stellar.hpx;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.io.InputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import android.graphics.PointF;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.opengl.Matrix;
import android.util.Log;
import android.view.MotionEvent;

import android.os.SystemClock;

public class VisualizationRenderer implements GLSurfaceView.Renderer {
	private static final String TAG = "Visualization Renderer";
	
	private FloatBuffer mVertexBuffer = null; 

    private PointF lastPoint;
	
	private List<PointF> points;

    boolean clear_after_render = false;
	
    private FloatBuffer vertexBuffer;
    // Left = -1.0
    // Right = 1.0
    // Bottom = 0.5
    // Top = -0.5
    // Near = 5
    // Far = 2000

    GLSurfaceView parent_;

    Context context;

    long startTime = 0;
	
	public VisualizationRenderer(GLSurfaceView parent, Context ctx) {
        Log.i("VisualizationRenderer", "constructing ...");
        parent_ = parent;
		points = new ArrayList<PointF>();
		points.clear();

        context = ctx;
        startTime = System.currentTimeMillis();
	}

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.i("VisualizationView", "onSurfaceCreated");
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
        GLES20.glEnable(GLES20.GL_CULL_FACE);

        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glEnable(GLES20.GL_TEXTURE_2D);

        /*
        GLES20.glAlphaFunc(GLES20.GL_GREATER, 0.7f);
        GLES20.glEnable(GLES20.GL_ALPHA_TEST);
        */

        int textures[] = new int[1];

        GLES20.glGenTextures(1, textures, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textures[0]);

        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_NEAREST);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D,
                GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_REPEAT);

        InputStream is = context.getResources()
            .openRawResource(R.raw.brushstrokes);
        Bitmap bitmap = null;
        try {
            bitmap = BitmapFactory.decodeStream(is);
        }
        catch(Exception e) {
            Log.e(TAG, "oh oh ...\n");
        }
        finally {
            try {
                is.close();
            } catch(IOException e) {
            }
        }

        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);//Bitmap.createBitmap(bitmap, 0, 0, 300, 100), 0);
        bitmap.recycle();

        init(textures[0]);
    }

    class Point3
    {
        Point3(float xx, float yy, float zz)
        {
            x = xx; y = yy; z = zz;
        }

        float x;
        float y;
        float z;
    }

    @Override
    public void onDrawFrame(GL10 gl) {

    	GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT); 
         
        renderParticles();
        
        System.gc();
    }

    int width = 0;
    int height = 0;

    @Override
    public void onSurfaceChanged(GL10 gl, int w, int h) {
        Log.i("...", "on surface changed");
        // Adjust the viewport based on geometry changes,
        // such as screen rotation
        //changed(width, height);
        float screenRatio = (float)w/h;
        float worldRatio = 320.f/240.f;
        if(worldRatio > screenRatio)
        {
            int viewportHeight = (int)(w/worldRatio);
            viewport[0] = 0;
            viewport[1] = (h - viewportHeight)/2;
            viewport[2] = w;
            viewport[3] = viewportHeight;
            GLES20.glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        else
        {
            int viewportWidth = (int)(h*worldRatio);
            viewport[0] = (w - viewportWidth)/2;
            viewport[1] = 0;
            viewport[2] = viewportWidth;
            viewport[3] = h;
            GLES20.glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        width = viewport[2] - viewport[0];
        height = viewport[3] - viewport[1];
        
        setLineView(gl);
        setParticleView(gl);

        /*
        GLES20.glHint(GLES20.GL_PERSPECTIVE_CORRECTION_HINT, GLES20.GL_NICEST);
        GLES20.glDepthMask(true);
        */
        changed(width, height, projMatrix);
    }

    private float[] projMatrix = new float[16];
    private float[] forceMatrix = new float[16];
    
    private float[] modelview = new float[16];
    private int[] viewport = new int[4];

    void setParticleView(GL10 gl)
    {
        float ratio = (float)height/width;
        Matrix.setIdentityM(projMatrix, 0);
        Matrix.frustumM(projMatrix, 0, -1.0f/ratio, 1.0f/ratio, ratio, -ratio, 5, 1000);
    }


    void setLineView(GL10 gl)
    {
        float left = 0.0f;
        float right = 320.0f;
        float bottom = 240.0f;
        float top = 0.0f;
        float near = -10.0f;
        float far = 10.0f;
        Matrix.setIdentityM(modelview, 0);
        Matrix.setIdentityM(forceMatrix, 0);
        Matrix.orthoM(forceMatrix, 0, left, right, top, bottom, near, far);
    }
    private PointF getWorldPoint(PointF p, float z)
    {
        float[] pos = new float[4];
        GLU.gluUnProject(p.x, p.y, 0.0f, modelview, 0, forceMatrix, 0, viewport, 0, pos, 0);

        //Log.i(TAG, pos[0] + " " + pos[1] + " " + pos[2]);

        return new PointF(pos[0], pos[1]);
    }
    
    public void onTouchEvent(MotionEvent e) {
    	//if(clear_after_render) return;

    	switch(e.getAction()) {
    	case MotionEvent.ACTION_MOVE:
            {
                //Add the points for the line to the buffer.
                //Log.i(TAG, "Action Move");
                PointF tmp = getWorldPoint(new PointF(e.getX(), e.getY()), 0.0f);
                /*
                PointF vec = new PointF(tmp.x - lastPoint.x, tmp.y - lastPoint.y);
                double distance = java.lang.Math.sqrt(vec.x * vec.x + vec.y * vec.y);
                */
                //Log.i(TAG, "Distance " + distance);
                //if(distance > 1.0)
                {
                    //Log.i(TAG, "???");
                    /*
                    PointF origin = getWorldPoint(lastPoint, 0.0f);
                    PointF dest = getWorldPoint(tmp, 0.0f);
                    */
                    newForce(tmp.x, tmp.y, tmp.x - lastPoint.x, tmp.y - lastPoint.y);
                    lastPoint = tmp;
                    //Log.i(TAG, lastPoint.x + " " + lastPoint.y);
                    //points.add(getWorldPoint(lastPoint, 0.0f));
                    //parent_.requestRender();
                }
            }
    		break;
    	case MotionEvent.ACTION_UP:
            {
                //Log.i(TAG, "Action UP");
                PointF tmp = getWorldPoint(new PointF(e.getX(), e.getY()), 0.0f);
                /*
                PointF vec = new PointF(tmp.x - lastPoint.x, tmp.y - lastPoint.y);
                double distance = java.lang.Math.sqrt(vec.x * vec.x + vec.y * vec.y);
                */
                //Log.i(TAG, "Distance " + distance);
                //if(distance > 1.0a)
                {
                    //Log.i(TAG, "???");
                    newForce(tmp.x, tmp.y, tmp.x - lastPoint.x, tmp.y - lastPoint.y);
                    //lastPoint = tmp;
                    //Log.i(TAG, lastPoint.x + " " + lastPoint.y);
                    //points.add(getWorldPoint(lastPoint, 0.0f));
                }
                lastPoint = null;
                //clear_after_render = true;
                //parent_.requestRender();
                //Log.i(TAG, lastPoint.x + " " + lastPoint.y);
            }
    		break;
    	case MotionEvent.ACTION_DOWN:
            //points.clear();
    		//Log.i(TAG, "Action Down");
            //Log.i(TAG, "!!");
            lastPoint = getWorldPoint(new PointF(e.getX(), e.getY()), 0.0f);
            //Log.i(TAG, lastPoint.x + " " + lastPoint.y);
            //points.add(getWorldPoint(lastPoint, 0.0f));
            //parent_.requestRender();
    		break;
    	default:
    		Log.wtf("Renderer", "Not Supposed to be here.");
    		Log.wtf("Renderer", "Bad Action" + e.getAction());
    		break;
    	}
    }

    private native void init(int texture);
    private native void renderParticles();
    private native void changed(int width, int height, float[] mat);
    private native void newForce(float originX, float originY, float dirX, float dirY);
    
}
