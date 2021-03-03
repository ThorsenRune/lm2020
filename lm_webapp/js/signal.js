//Plotting the signal
/*
	Premises: A 'signal.HTML' file with
		wss1				Container element
		idSignalTitle		Title element
		idMaxVal,idMinVal	Y axis limits

*/

"use strict";
var eSVG, path1,lMidline;		//TODO: put in signal object
var len=100;
var ampl=1000;
/*				SIGNAL OBJECT public methods									*/
var signal={}
signal.init=function(){
		eSVG=document.querySelector("svg")
		path1=eSVG.querySelector('#idSignal');
		lMidline=eSVG.querySelector('#idMidLine');
		lMidline.style.strokeDasharray="10 30"
		eSVG.ontouchstart	= function(){mFocus(this)}
		eSVG.onclick		= function(){mFocus(this)}
	}
signal.id=function(n){
		return eSVG.parentElement.id;
	}
signal.Title=function(str){//Remember id='idSignalTitle' in signal.html
		window.idSignalTitle.innerHTML=str;  //Todo: softcode
	}



signal.mPlotVector=function(Y,min,max,units){
	var len=Y.length
	if(len<2) return;
	eSVG.setAttribute('preserveAspectRatio', 'none');
	//viewBox = <min-x> <min-y> <width> <height>
	var range=max-min;
	eSVG.setAttribute('viewBox',"0  0 "+len+" "+range);
	var top=max			//Flip yaxis
	//Center origo at midline
	//Centerline
	lMidline.setAttribute("d"," M0,"+(top-0)+" L"+len+","+(top-0));

	var path= "M0,"+(top-Y[0]);
	for (var i = 1; i < len; i++) {
		path=path+" L"+i+","+(top-Y[i]);	//Flip yaxis
	}
	path1.setAttribute("d", path)
	window.idMaxVal.textContent=max+units;
	window.idMinVal.textContent=min+units;
}
