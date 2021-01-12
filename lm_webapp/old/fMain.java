//Doc:  https://docs.google.com/document/d/1BpYIuSab_7Fx0CpxHqXR0yQIb85Qv8Js-EHtS6rRB0I/edit
//171006    Total Refactoring
package it.fdg.lm190925web;

//170919        170912 kAutoconnect parameter on restart
//folder: https://drive.google.com/drive/u/0/folders/0B5pCUAt6BabuU1I1Ri1zNzA4SG8
//170823    Rewriting loading of the widget settings

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.HorizontalScrollView;

import java.util.ArrayList;

import static it.fdg.lm190925web.cAndMeth.mSetVisibility;
import static it.fdg.lm190925web.cProgram3.bDoRefresh;
import static it.fdg.lm190925web.cProgram3.mAppSettings;
import static it.fdg.lm190925web.cProgram3.mAppSettings_BootLoad;
import static it.fdg.lm190925web.cProgram3.mCommunicate;
import static it.fdg.lm190925web.cProgram3.mEndProgram;
import static it.fdg.lm190925web.cProgram3.mPersistAllData;
import static it.fdg.lm190925web.cProgram3.mySignal;
import static it.fdg.lm190925web.cProgram3.nPanelSizes;
import static it.fdg.lm190925web.cProgram3.oGlobalGestureDetector;
import static it.fdg.lm190925web.cProgram3.oUInput;
import static it.fdg.lm190925web.cProgram3.oaProtocols;
import static it.fdg.lm190925web.cProgram3.sDevices2;
import static it.fdg.lm190925web.cProgram3.sFile_ProtCfg;
import static java.lang.Integer.signum;


public class fMain extends BaseActivity {

    public static ViewGroup fPanelHorizSliders,fPanelVertSliders,fPanelSignals1,fPanelData, fVertSliderPane1;
    public static ViewGroup mySliderPane; //Panel of the sliders
    private static Button cmdCommand1;
     private int  nTextSize=0;
    static GestureDetector mGestureDetector;
    private boolean debug=false;

    //******************************    MAIN METHODS  ENTRYPOINT
    public void mEntryPoint() {
        //!?mContext = this;
        fVertSliderPane1 = (ViewGroup)    findViewById(R.id.idHorizontalScroll);
        fPanelHorizSliders = (ViewGroup)    findViewById(R.id.idContainer4HorizontalSliders);
        fPanelVertSliders = (ViewGroup)     findViewById(R.id.idVerticalSliderPane);
        mySliderPane = (ViewGroup)          findViewById(R.id.idVerticalSliderPane);        //Container for the sliders
        mySignal = (cSignalView2)           findViewById(R.id.idSignalView);
        cmdCommand1 = (Button)              findViewById(R.id.idCommand);

        fPanelSignals1 = (ViewGroup)        findViewById(R.id.idContainer4Signals1);
        fPanelData = (ViewGroup)            findViewById(R.id.idContainer4Data1);
        cAndMeth.mInit(this);       //Initialize the general methods
        mInit(this);                //Will only run first time ignoring second calls
        mInitControls();                    //Register  widgets in cProgram3.oaWidgetList();
        oUInput.mInit(this);         //  initialise the input module
        cKonst.nTextSize = (int) (mGetTextSize() );     //12 points
        cProgram3.oGraphText = new cGraphText();
        cProgram3.oGraphText.mInit((int) cKonst.nTextSize);         //Enable drawing of texts

        //cProgram3.mPrivileges(0);     //180328B
        if (sDevices2[0].length()<1) cProgram3.mLoadFactorySettings(sFile_ProtCfg);          //first run

    }
obsolee mInit(Context mainContext){     //cProgram3
        if (oaProtocols !=null)
            return;        //Return if its not the first call (second calls could be caused by rotating the device)
        cProgram3.mContext=mainContext;
        cFileSystem.mInit(mainContext);
        //Prepare the protocols
        mAppSettings(true);
        if (sFile_ProtCfg=="") mAppSettings_BootLoad();
        mPersistAllData(true,sFile_ProtCfg);         //onCreate
    }

    private static void mLoadFactoryDefault() {
        oUInput.mSelectFactoryFile();
    }

    public void mInitControls() {      //Setup widget references for this display
  /*     initiate  views        */
       cProgram3.oTextTypeface =cmdCommand1.getTypeface();
        mySignal.mInit(1);      //One signal pane
//       mCloneView(view ,7);   //Make add 8 sliders 170914

        ArrayList<View> aW = cAndMeth.mAllChildViews(null, (ViewGroup) findViewById(android.R.id.content));
        cSlider.nSliderCount=0;         //Initialize the sliderarray 170915
        for (int i=0;i<aW.size();i++){      //Iterate trhoug all views in activity to assign controls
            if (aW.get(i) instanceof cSliderView ){
                cSlider.mAddSlider(this,(cSliderView) aW.get(i)); //Add new slider to sliderarray
                mTouchListening( aW.get(i));     //Set the touchhandler
            }

        }
        mAddControl("WD1",R.id.idWD1);          //Set Data views
        mAddControl("WD2",R.id.idWD2);
        mAddControl("WD3",R.id.idWD3);
        mSetTouchListeners();


    }       //Initialize controls on this view

    private void mAddControl(String sID, int idWD) {   //171130    Add a control to the control array
        cData_View v = (cData_View) findViewById(idWD);
        v.mInit(this,sID    );
        cProgram3.oControls_Add(v,sID);
        mTouchListening(v);
    }


    private void mSetTouchListeners() {
        oGlobalGestureDetector =new cGestureListener(this);   //170915

        mTouchListening(cmdCommand1);
        mTouchListening((View) fPanelVertSliders);     //Set the touchhandler
       // mTouchListening((View) fPanelVertSliders.getParent());     //Set the touchhandler
        // mTouchListening((View) mySliderPane.getParent());     //Set the touchhandler
        mTouchListening((View) fPanelHorizSliders);     //Set the touchhandler
      //  mTouchListening((View) fPanelSignals1);     // Signal touch
        mTouchListening(mySignal);           // Signal touch handler
        int count = mySliderPane.getChildCount();
        for (int i = 0; i < count; i++) {
            View child = mySliderPane.getChildAt(i);
            //mTouchListening(child);
            if (child instanceof HorizontalScrollView){
                //mTouchListening(child);
            }
        }
    }



    private void mTouchListening(final View view) {     //Activated for each control that can be activated by touch
            view.setOnTouchListener(new View.OnTouchListener() {         //Touch event for the watch pane
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                //Respond to touch events. if a false is returned the event will stop and not progress
                boolean bRetVal=false;
                boolean  bRetVal1 = v.onTouchEvent(event);              //Trigger the class events
                if (v.equals(fPanelVertSliders))                        //Action VERTICAL SLIDER
                    bRetVal= oGlobalGestureDetector.mListen(   fPanelVertSliders, event);       //Get extended gestures
                else if (view.equals(cmdCommand1)) {                    //ACTION ON COMMAND
                    bRetVal= oGlobalGestureDetector.mListen(v,event);           //Get the actions
                    if (oGlobalGestureDetector.bInputGesture()){
                        cUInput.mCommand(true   );
                    }else if (oGlobalGestureDetector.bScrolling) {
                        oUInput.mChange(oGlobalGestureDetector.nChange);
                        oGlobalGestureDetector.bScrolling = false;
                    }else if (oGlobalGestureDetector.bDoubleTap()){
                        oUInput.mZero();
                    } else if (oGlobalGestureDetector.bFlingLR()){
                        if (cProgram3.bEnableZeroByFling) oUInput.mZero();
                    } else if (oGlobalGestureDetector.bLongPress()) {
                        if (cProgram3.bEnableCommandLongPress) oUInput.mCommandLongPress();
                    } else if (oGlobalGestureDetector.bSingleTap()) {
                        cUInput.mCommand(false);        //Can only connect
                    }else {
                        Log.d("myLog", "180927:Not implemented ");
                    }
                } else if (view.equals(fPanelVertSliders)) {
                    if (oGlobalGestureDetector.bFlingLR()){
                        mShiftWatchPane(signum(oGlobalGestureDetector.nFlingDir[0]));
                    } else {
                        if (debug)   bRetVal=true;          //Event consumed
                    }
                } else if (view.equals(fPanelSignals1.getParent()) ) {


                } else if (view instanceof HorizontalScrollView) {

//                } else if ((view instanceof cSignalView2)||(view.equals(fPanelSignals1))) {
                } else if ((view instanceof cSignalView2)) {
                    bRetVal= oGlobalGestureDetector.mListen(   view, event);
                    cUInput.mSetFocus(view);
                    if (oGlobalGestureDetector.bLongPress())
                        //nRefreshRate(5000);      //Slow down the refresh rate
                    if (oGlobalGestureDetector.bSingleTap()) {
                        ((cSignalView2)view).mShowCoord(oGlobalGestureDetector.nX, oGlobalGestureDetector.nY);

                    }
                    if (oGlobalGestureDetector.bInputGesture()) {           //Long or doubletap a slider will activate value input
                        oUInput.mInputRange();

                    }else if (oGlobalGestureDetector.bFlingLR()){
                        mySignal.mShiftPane(signum(oGlobalGestureDetector.nFlingDir[0]));  //Change the pane in direction -1,1
                    }else if (oGlobalGestureDetector.bScaling()) {
                        mySignal.oElemViewProps().mZoom(oGlobalGestureDetector.nScaleCenterX, oGlobalGestureDetector.nScaleCenterY, oGlobalGestureDetector.nScaleFactor);
                        mySignal.oElemViewProps().myProtElem1().mCentreAround(0);
                    }

                }else if ((view instanceof cSliderView)) {
                    bRetVal= oGlobalGestureDetector.mListen(   view, event);    //NOtify of focus on this
                    cUInput.mSetFocus(view);
                    cSliderView V = (cSliderView) view;
                    if (oGlobalGestureDetector.bScaling()) {       //Check global gestures first
                        if (cUInput.mGetViewProps().bZoomAble())
                        cUInput.mGetViewProps().mZoom(oGlobalGestureDetector.nScaleCenterX, oGlobalGestureDetector.nScaleCenterY, oGlobalGestureDetector.nScaleFactor);
                    } else if (oGlobalGestureDetector.bInputGesture()) {           //Long or doubletap a slider will activate value input
                        oUInput.mInputValue1();
                    } else if (oGlobalGestureDetector.bFlingDown()){
                        if (V.bRotate)  //Only fling zero vertical sliders. 180417A
                           oUInput.mZero();
                    } else if (oGlobalGestureDetector.bDoubleTap()){
                            oUInput.mZero();
                    } else {
                        bRetVal = V.onTouchEvent(event);              //Perform sliding movements
                    }
                } else {
                    Log.d("myLog", "181022:Not implemented ");
                }
                if (bRetVal) {
                    Log.d("mTouchListening", "True");
                }else
                    Log.d("mTouchListening", "False");
                return bRetVal;
            }
        });
    }
    private void mShiftWatchPane(int signum) {
      //  cProgram3.nWatchPage=mLimit(0,(cProgram3.nWatchPage+signum),1);
        cProgram3.bDoRedraw=true;
    }



    public static void mRefresh_DispMain(boolean doRedraw) {       //Refresh controls on display
        if (cProgram3.mySignal==null) return;   //Display not ready
        if (doRedraw) mRedraw();
        if (doRedraw){
            cProgram3.mySignal.mInit(1);
        }
        if (cProgram3.oFocusdActivity instanceof cSetupFile){
//            ((cSetupFile)oFocusdActivity).mDispRefresh(doRedraw);
        } else if (cProgram3.oFocusdActivity instanceof cBitField) {
            ((cBitField) cProgram3.oFocusdActivity).mRefresh(doRedraw);
        } else {
            cProgram3.mControlsRefresh(doRedraw);
            for (int i = 0; i < cSlider.nSliderCount; i++) {
                cProgram3.oSlider[i].mRefresh(doRedraw);
            }

            if (cProgram3.mySignal != null) {
                cProgram3.mySignal.mRefreshSignal(doRedraw);
            }
           if (bDoRefresh) oUInput.mCommand(false); //Refresh the command line
        }
    }

    private static void mRedraw() {     //Redrawing the main window
        cAndMeth.mLayoutWeightSet(fVertSliderPane1, nPanelSizes[0]);

        cAndMeth.mLayoutWeightSet(fPanelHorizSliders, nPanelSizes[1]);

        mSetVisibility(fPanelSignals1,cProgram3. nPanelSizes[2]>0);
        cAndMeth.mLayoutWeightSet(fPanelSignals1, nPanelSizes[2]);
        mySignal.setVisibility(fPanelSignals1.getVisibility());

        mSetVisibility(fPanelData,cProgram3.bPanelData);
        cAndMeth.mLayoutWeightSet(fPanelData, nPanelSizes[3]);

        //mScrollViewBlock(fVertSliderPane1);

        mStatusRedraw();
    }
/*
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        oGlobalGestureDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
        // Return true if you have consumed the event, false if you haven't.
        // The default implementation always returns false.
    }
*/
    public static void mCommandSet( String sMsgCmd) {
     /*   Handler handler = new Handler(Looper.getMainLooper());
        handler.post(new Runnable() {
            public void run() {
                if (sMsgCmd == "")
                    mSetVisibility(cmdCommand1, 0);
                else {
                    mSetVisibility(cmdCommand1, 1);
                    cmdCommand1.setText(sMsgCmd);
                }
            }
        });

      */
    }


    //  *************************                    SYSTEM STUFF       *************************
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.fmain);
        cProgram3.mInit(this);
        mCommandSet("Starting LM");
        mInitWindow();
    }


    public void mInitWindow() {
        cProgram3.oFocusdActivity=this;             //Default value for acti
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        mEntryPoint();
    }

    @Override
    public void onResume() {
        super.onResume();
        cProgram3.bDoRedraw=true;             //Update the screen
        cProgram3.oFocusdActivity=this;
        mCommunicate(true);
    }
    @Override
    protected void onDestroy() {            //Last event before program is killed, but also called sometimes when another activity is ending?
//        if (isFinishing())             mAlert2("Finishing onDestroy");
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {       //Some black magic asking user how to quit, saving/not saving data
        mEndProgram();
        super.onBackPressed();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (isFinishing()) {

        }
    }
    @Override
    protected void onStop() {
        super.onStop();
        if (isFinishing()) {
            mEndProgram();
        }
    }


    public int mGetTextSize() {
        if (nTextSize==0)
        nTextSize= (int) cmdCommand1.getTextSize();
        //nTextSize= getResources().getDimensionPixelSize(R.dimen.myFontSize);
        return nTextSize;
    }

    public static void cmdText(String s) {      //Set text for command button
        if (s=="")
           s=".";
        mCommandSet(s);
        bDoRefresh=true;
    }


}
