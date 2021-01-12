package it.fdg.lm190925web

import android.os.Handler
import android.os.Looper
import android.support.v4.os.HandlerCompat.postDelayed
import org.jetbrains.annotations.NotNull


class cProgram4 {
    private lateinit var oMain: ActivityWebView

    //    val oMain=ActivityWebView();
    private val bRunning2 = true
    var nRefreshRate =  5000L
//    private val handler = Handler()
    lateinit var mainHandler: Handler
    private val updateTextTask = object : Runnable {
        override fun run() {
            mMainProcess1()
            if (bRunning2)
                mainHandler.postDelayed(this, nRefreshRate)
        }
    }
    //------------------------------------


    fun mChangeRefreshRate() {
        nRefreshRate = nRefreshRate / 2
        this.mPeriodicProcessingrun()      //Start the timer

    }

    private fun mPeriodicProcessingrun() {
        if (::mainHandler.isInitialized) return;
        mainHandler = Handler(Looper.getMainLooper())
        updateTextTask.run();

    }

    private fun mMainProcess1() {      //Where data are updated
        oMain.myCounter = oMain.myCounter + 1
        val s = "Timer:" + oMain.myCounter
        oMain.mAnd2JS(s)
    }

    private fun mMessage(stopped: String) {
        oMain.showToast(stopped)
    }

    fun mInit(activityWebView: ActivityWebView) {
        oMain=activityWebView;
    }


}