"use strict";
let slider = (() => {
// Getting and setting values
//element.label is the label for the control
//element.value is the value in %

// Get them as slider.inputs[]
	function id(n){
		return this.inputs[n].parentElement.id;
	}
	function getValue(n){
		var element=this.inputs[n]
		return Number(element.value);
	}
	function setValue(n,val ){
		var element=this.inputs[n]
		if (typeof val !=='number') debugger
		if(Number(element.value)==val) 	return; //No change
		element.value=val;
		updateSlider(element)
	}
	function setRange(n,range,unit,label){
		var element=this.inputs[n]
		mSettings(element,range,unit,label)
	}
	function mValue(element,newVal){//191108
	//Set value by element
		if (newVal!==undefined) {
			if (typeof newVal !=='number') debugger
			if(Number(element.value)==newVal) 	return; //No change
			element.value=newVal;
			updateSlider(element)
		}
		return Number(element.value);
	}
	function mSettings(element,range,unit,label){//191108
	//Set slider properties by input element
		element.unit	=unit;  	//unit label
		element.label	=label;
		element.min= range[0]
		element.max= range[1]
		updateSlider(element)
	}
// 
  function updateSlider(element,bUserInput) {
    if (element) {
		let parent = element.parentElement;
	//	if (lastValue == element.value)      return; // No value change, no need to update then
		//var relval=element.value;
		var relval=Number(element.value).norm([element.min,element.max]);
	if (bUserInput)	{
		element.rawval=Number(element.value).unnorm(element.range)
		 
	}
      //parent.setAttribute('data-slider-value', element.value);
	  parent.setAttribute('data-slider-value', element.label);
      let $thumb = parent.querySelector('.range-slider__thumb'),
      $bar = parent.querySelector('.range-slider__bar'),
      pct = relval * ((parent.clientHeight - $thumb.clientHeight) / parent.clientHeight);
	//see https://www.w3schools.com/tags/att_input_type_range.asp
      $thumb.style.bottom = `${pct}%`;  //Backticks   literals allowing embedded expressions. https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Template_literals
      $bar.style.height = `calc(${pct}% + ${$thumb.clientHeight / 2}px)`;
      $thumb.textContent = `${element.value}${element.unit}`;
		if (bUserInput) element.bPokeValue=true;
    }
  }
  return {			//Export the variables
    updateSlider: updateSlider //Redraw the control
    ,getValue: getValue  		//Get current value
	,setValue:setValue 			//Set value and update control
	,id		:id
	,setRange:setRange
	,mSettings:mSettings		//Setup the slider  191108
	,mValue:mValue				//Get/Set value 191108
	};


})();

(function initAndSetupTheSliders() {
 //http://www.javascriptkit.com/javatutors/arrayprototypeslice.shtml
 //Convert the controls into an array
  const inputs = [].slice.call(document.querySelectorAll('.range-slider input'));
  slider.inputs=inputs;
  //inputs.forEach(input => input.unit='%');  //Default to a percentage unit
   // Cross-browser support where value changes instantly as you drag the handle, therefore two event types.
  inputs.forEach(input => input.addEventListener('input', element => slider.updateSlider(input,true)));
  inputs.forEach(input => input.addEventListener('change', element => slider.updateSlider(input,true)));
  inputs.forEach(input => input.addEventListener('touchstart',  function(){mFocus(this)})); 
  inputs.forEach(input => input.addEventListener('focus',  function(){mFocus(this)})); 
  for (var i = 0; i < slider.inputs.length; i++) {
	  slider.inputs[i].value=(i+1)*10;
	  slider.inputs[i].range=[	0,100];  	//Default range
	  slider.inputs[i].unit='u';  		//Default to a percentage unit
	  slider.inputs[i].label='name';
	  slider.inputs[i].myIndex=i;
	  slider.updateSlider(slider.inputs[i]);
  }
})();