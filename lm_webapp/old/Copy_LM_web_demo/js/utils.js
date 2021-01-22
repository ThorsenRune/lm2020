"use strict";




function mFillDataList(elInput,itemlist){		//Make the dropdown list from a list [,,,]
	//Revised 190920	
		if (elInput.tagName!="INPUT") {debugger; //an error The Containing "INPUT" element was not given as argument.
		//if (dataList==document.activeElement) return; //Dont populate if this has 	focus
		} 
		elInput.autocomplete='off'; //Stop rememberting onld values
		elInput.removeAttribute('list')//Remove previous list
		var datalist =document.createElement('datalist');
		datalist.id =elInput.id+'list';
		itemlist.forEach(function(item) {
			 // Create a new <option> element.
			 var option = document.createElement('option');
			 // Set the value using the item in the JSON array.
			 option.value = item;
			 // Add the <option> element to the <datalist>.
			 datalist.appendChild(option);
		});
		elInput.appendChild(datalist);
		elInput.setAttribute('list',datalist.id);
	}

Array.prototype.add = function( b ) {
    var a = this,
        c = [];
    if( Object.prototype.toString.call( b ) === '[object Array]' ) {
        if( a.length !== b.length ) {
            throw "Array lengths do not match.";
        } else {
            for( var i = 0; i < a.length; i++ ) {
                c[ i ] = a[ i ] + b[ i ];
            }
        }
    } else if( typeof b === 'number' ) {
        for( var i = 0; i < a.length; i++ ) {
            c[ i ] = a[ i ] + b;
        }
    }
    return c;
};
Array.prototype.div=function(b)  {
	var a = this,
        c = [];
	if( typeof b === 'number' ) {
        for( var i = 0; i < a.length; i++ ) {
            c[ i ] = a[ i ] / b;
        }
    }
    return c;
};
Array.prototype.sub=function(b)  {
	var a = this,
        c = [];
	if( typeof b === 'number' ) {
        for( var i = 0; i < a.length; i++ ) {
			if (a[i]===undefined)a[ i ]=0;
            c[ i ] = a[ i ] - b;
        }
    }
    return c;
};
Array.prototype.mean=function()  {
	var a = this,sum=0,        c = [];
	for( var i = 0; i < a.length; i++ ) {
		sum += a[ i ] ;
	}
    return sum/a.length;
};
Array.prototype.max=function()  {
	var a = this;
	var y=a[0];
	for( var i = 0; i < a.length; i++ ) {
		if (a[ i ]>y)y=a[i] ;
	}
    return y;
};
Array.prototype.min=function()  {
	var a = this;
	var y=a[0];
	for( var i = 0; i < a.length; i++ ) {
		if (a[ i ]<y)y=a[i] ;
	}
    return y;
};
Array.prototype.mult=function(b)  {
	var a = this,
        c = [];
	if( typeof b === 'number' ) {
        for( var i = 0; i < a.length; i++ ) {
            c[ i ] = a[ i ] * b;
        }
    }
    return c;
};

Number.prototype.norm=function(range){	//Normalize to percentage
	var x=this;
	var y=100*(x-range[0])/(range[1]-range[0]);
	return y
}
Number.prototype.unnorm=function(range){	//Normalize to percentage
	var x=this;
	var y=range[0]+((range[1]-range[0])*x)/100;
	return Number(y)
}
	
 String.prototype.replaceAll = function(search, replace) {//Replace all occourences of <search> with <replace> in the string
	if (replace === undefined) {
		return this.toString();
	}
	return this.split(search).join(replace);
}	

Object.prototype.size=function(){
	 return Object.keys(this).length
}
 