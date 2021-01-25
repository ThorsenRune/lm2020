 //	todo: move into prot object

 
/*	
a=Number.parseInt(slider.getValue(1))/20;
		k=Number.parseInt(slider.getValue(0));
		b=Number.parseInt(slider.getValue(2))/500;
 *//*
function mtest(){
	var myvar=  {
		_a1:123 
		,_n:function(){return Math.random()}() 		//Initializing n once
		,get: function a(k)	{				return this._a1*k ;		}
		,set a(val)	{				this._a1=val;				}
	}
 	return myvar;
}
x=mtest();y=mtest();x.a=1;y.a=2;x.n



Object.defineProperty(myvar,'bar',{
	get bar(){ return 123}
	});


*/

		function tempclean(oWX){  //todo remove . function
			if( oWX.range) oWX.Range= oWX.range;
			if( oWX.range) delete(oWX.range)		
			delete(oWX.oVar)
			prot.pokedata()
		}