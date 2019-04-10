/*******************************************************************************
錯誤設置
*******************************************************************************/
function errmsg(message,url,line) {
  amsg = "Sorry! A JavaScript error has occurred. Please let us know about it.\n\n";
  amsg += "Error:\n" + message + "\n";
  amsg += "URL:\n" + url + "# " + line;
  alert(amsg);
  return false;
}
window.onerror=errmsg;
/*******************************************************************************
主題，語言
*******************************************************************************/
function ThemeClass(){
	var themeIndex = 0;
	var languageIndex = 0;	// 0 en 1 jp 2 tw
	var showPagetop = 0;	// 0 default 1 on 2 off
	var pagetop;
	var asideheight;
	var aside;
	
	this.backgroundColor = new Array();
	this.foregroundColor = new Array();
	
	this.weekName = [
		['Sun','Mon','Tue','Web','Thu','Fri','Sat'],
		['日','月','火','水','木','金','土'],
		['日','一','二','三','四','五','六']
	];
	this.backtotopchar = ['top','top','返回'];
	
	
	this.themeInit = function(){
		this.showPagetop = 0;
		this.themeIndex  = 0;
		this.asideheight = 10;
		this.languageIndex = 0;
		this.aside = window.document.getElementsByTagName("aside")[0];
		this.asideheight = this.aside ? this.aside.offsetTop : 0;
		
		this.backgroundColor[0] = window.getComputedStyle(document.body, null).backgroundColor;
		this.backgroundColor[1] = "pink";
		this.backgroundColor[2] = "black";
		
		this.foregroundColor[0] = window.getComputedStyle(document.body, null).color;
		this.foregroundColor[1] = "#669";
		this.foregroundColor[2] = "#EEE";
	}
	this.changeTheme = function(){
		//alert(themeIndex);
		themeIndex++;
		if(themeIndex > this.backgroundColor.length-1)themeIndex=0;
		window.document.body.style.backgroundColor = this.backgroundColor[themeIndex];
		window.document.body.getElementsByTagName("content")[0].style.color = this.foregroundColor[themeIndex];
		
	}
	this.SetPagtop = function(){
		if(!this.pagetop)return;
		this.pagetop.style.display = "block";
		if(!(document.body.scrollTop > 50 || document.documentElement.scrollTop > 50)){
			this.pagetop.style.display = "none";
			//console.log("_1");
		}
		if(this.showPagetop == 0 && document.body.clientWidth < 640){
			this.pagetop.style.display = "none";
			//console.log("_2");
		}
		if(this.showPagetop == 2){
  		this.pagetop.style.display = "none";
			//console.log("_3");
		}
		//console.log(this.pagetop.style.display + " " + this.showPagetop);
		this.pagetop.style.right = (parseInt(window.getComputedStyle(document.body.getElementsByTagName("content").item(0), null).marginRight, 10)+20) + "px";
	}
	
	this.SetAside = function(){
		if(!this.aside)return;
		if(this.asideheight == 0)this.asideheight = this.aside.offsetTop;
		var top  = window.pageYOffset || document.documentElement.scrollTop;
		var left = window.pageXOffset || document.documentElement.scrollLeft;
		
		if(top > this.asideheight-20){
			//this.aside.style.marginTop = (top-this.asideheight+20) + "px";
			this.aside.style.position = "fixed";
			this.aside.style.top = "20px";
			this.aside.style.right = (parseInt(window.getComputedStyle(document.body.getElementsByTagName("content").item(0), null).marginRight, 10)) + "px";
			//this.aside.style.right = document.body.getElementsByTagName("content").item(0).width+document.body.getElementsByTagName("content").item(0).offsetLeft + "px";
			console.log(this.aside.style.right);
		}else{
			//this.aside.style.marginTop = 0;
			this.aside.style.position = "static";
		}
		//console.log(this.aside.style.marginTop + ", " + this.asideheight);
	}
	
	this.setContent = function(){
		var footerHeight = window.document.getElementsByTagName("footer").item(0).getBoundingClientRect().height;
		//var footerOffset = window.document.getElementsByTagName("footer")[0].offsetTop;
		var body = document.body;
		var html = document.documentElement;
		var bodyHeight = Math.min( body.scrollHeight, body.offsetHeight, 
                       html.clientHeight, html.scrollHeight, html.offsetHeight );
		var footerOffset = bodyHeight - footerHeight;
		console.log(bodyHeight);
		var content = window.document.getElementsByTagName("content").item(0);
//		console.log(this.footerheight +","+ window.document.getElementsByTagName("footer").item(0).getBoundingClientRect().height);
//		console.log(document.body.clientHeight);
//alert(window.innerHeight+ "," + this.footerheight +","+ window.document.getElementsByTagName("footer").item(0).getBoundingClientRect().height);
		if(window.innerHeight > footerHeight + footerOffset){
			content.style.height = (content.getBoundingClientRect().height + window.innerHeight - (footerOffset + footerHeight)) + "px";
		}else{
			content.style.height = "auto";
		}
		//console.log(window.document.getElementsByTagName("content").item(0).getBoundingClientRect().height);
		//console.log(window.document.getElementsByTagName("content").item(0).style.height);
	}
	
}
var theme = new ThemeClass();
/*******************************************************************************
初始化
*******************************************************************************/
// init
window.onresize = function(){
	// set back-to-pagetop-positin
	theme.SetPagtop();
	theme.setContent();
}

window.onscroll = function(){
	//alert(document.body.scrollTop);
	theme.SetPagtop();
	theme.SetAside();
	//theme.setContent();
}

window.onload = function(){
	theme.themeInit();
	// back to top
	var ele = document.createElement("div");
	ele.setAttribute("style","display:bloack;border:1px solid #F90;border-radius: 40px;height: 80px;width: 80px;text-align: center;background-color: #FFF;position: fixed;bottom: 10px;font-size: 15px;cursor: pointer;text-align: center;line-height: 80px;  alignment-baseline:middle;");
	ele.textContent = theme.backtotopchar[theme.languageIndex+2];
	ele.setAttribute("onclick","window.scrollTo(0, 0);");
	/*ele.setAttribute("style","display: block;cursor: pointer;text-align: center;line-height: 20px;padding: 20px 0;margin: 0;");*/

	document.body.insertBefore(ele,document.body.lastChild);
	ele.style.right = (parseInt(window.getComputedStyle(document.body.getElementsByTagName("content").item(0), null).marginRight, 10)+20) + "px";
	
	theme.pagetop = ele;
	theme.SetPagtop();
	
  // control button
  var me = document.body.getElementsByTagName("article")[0];
	if(!me){
		//alert("none article");
		theme.pagetop.style.display = "none";
		theme.showPagetop = 2;
	}
  
	/*var btn = document.createElement("BUTTON");
  btn.appendChild(document.createTextNode("回頂開關"));
  btn.onclick = function(){
		theme.showPagetop = theme.pagetop.style.display == "none" ? 1 : 2;
		console.log(theme.showPagetop);
   	theme.pagetop.style.display = theme.showPagetop == 1 ? "block" : "none"; 
  }
	if(me && me.getElementsByTagName("h1")){
  	me.insertBefore(btn,me.getElementsByTagName("h1")[0].nextSibling);
	}
	
  var btn = document.createElement("BUTTON");
  btn.appendChild(document.createTextNode("吐槽開關"));
  btn.onclick = function(){
    for(var i=0;i<document.getElementsByTagName("S").length;i++)
    {
      document.getElementsByTagName("S").item(i).style.display = 
			document.getElementsByTagName("S").item(i).style.display == "none" ?
			"inline" : "none";
    }
  }
	if(me && me.getElementsByTagName("h1")){
  	me.insertBefore(btn,me.getElementsByTagName("h1")[0].nextSibling);
	}
  
  var btn = document.createElement("BUTTON");
  btn.appendChild(document.createTextNode("更換主題"));
	btn.setAttribute("onclick","theme.changeTheme()");
//  btn.onclick = "theme.changeTheme()";
	if(me && me.getElementsByTagName("h1")){
  	me.insertBefore(btn,me.getElementsByTagName("h1").item(0).nextSibling);
	}*/
  
  var btn = document.createElement("BUTTON");
  btn.appendChild(document.createTextNode("QR code"));
  btn.onclick = function(){
    //TODO: !
  }
  //me.insertBefore(btn,me.getElementsByTagName("h1").item(0).nextSibling);
  
  // 顯示代碼
  var ele = document.getElementsByTagName("pre");
  for(var i=0; i<ele.length; i++)
  {
    if(ele[i].firstChild.tagName == "CODE" && ele[i].getBoundingClientRect().height > 300){
      ele[i].style.display = "none";
      var btn = document.createElement("BUTTON");
      btn.appendChild(document.createTextNode("look"));
      btn.onclick = function(){
				  this.nextSibling.style.display = this.nextSibling.style.display =="none" ? "block" : "none";
					theme.setContent();
			};
      ele[i].parentNode.insertBefore(btn,ele[i]);
    }
  }
	
	for(var i=0; i<document.body.getElementsByTagName("del").length; i++)
  {

    var tag = document.body.getElementsByTagName("del").item(i);
    tag.style.display = "none";
  }

/*
  標籤標記，暫時不使用
  for(var i=0; i<me.getElementsByTagName("taglist").length; i++)
  {

    var tag = me.getElementsByTagName("taglist").item(i);
    
    //alert(tag.getElementsByTagName("tag").length);
    for(var j=0;j<tag.getElementsByTagName("tag").length;j++){
      //alert(j);
      tag.getElementsByTagName("tag")[j].style.backgroundColor = "#134";
      tag.getElementsByTagName("tag")[j].style.border = "1px solid #dcdcdc;";
    }
  }
*/
  
  // title nav
  var me = document.body.getElementsByTagName("header")[0];
  var a = document.createElement("A");
  a.setAttribute("href","https://frankrx41.github.io");
  a.insertBefore(me.getElementsByTagName("h1")[0],a.lastChild);
  a.insertBefore(me.getElementsByTagName("h2")[0],a.lastChild.nextSibling);
  me.insertBefore(a,me.firstChild);
  
	// add close button on aside
	var aside = document.body.getElementsByTagName("aside").item(0);
	var me = aside ? aside.getElementsByTagName("div") : null;
  for(var i=0;me && i<me.length; i++)
  {
		var ele = document.createElement("span");
		ele.setAttribute("style","margin-top:5px ;margin-right:10px; cursor:pointer; float:right; color: #C00; font-family: Consolas , sans-serif; font-size:20px;");
		ele.setAttribute("onmouseover","this.style.color = '#F00';");
		ele.setAttribute("onmouseout","this.style.color = '#C00';");
		ele.appendChild(document.createTextNode("✖"));
		ele.setAttribute("onclick","this.parentNode.style.display = 'none';");
		me[i].insertBefore(ele,me[i].firstChild);
  }
	
	// aside calendar
	var me = aside ? aside.getElementsByTagName("div") : null;
	var calendar;
	for(var i=0;me && i<me.length; i++)
  {
		//alert(me.item(i).getElementsByTagName("H1").item(0).textContent);
		if(me.item(i).getElementsByTagName("H1").length){
			if(me.item(i).getElementsByTagName("H1").item(0).textContent.toLowerCase() == "calendar")
			{
				calendar = me.item(i);
			}
			else{
				me.item(i).style.display = "none";
			}
		}
	}
	if(calendar){
		var table = document.createElement('table');
		table.setAttribute("style","margin: 0 auto; padding:0;");
		
		var tr = document.createElement('tr');
		for (var col = 0; col < 7; col++){
			var td = document.createElement('td');
			td.setAttribute("style","  vertical-align:middle; text-align:center; text-height:40px; font-size:14px; border:1px solid #ddd; border-radius:3px;height:40px; width:40px;white-space:pre;padding:0 auto;");
			td.textContent = theme.weekName[theme.languageIndex][col];
			tr.appendChild(td);
		}
		table.appendChild(tr);
				
		var date = new Date();
		var today = date.getDate();
		var date = new Date(date.getFullYear(),date.getMonth(),1);
		
		var first_day = date.getDay();// 一日の曜日
		var last_day = new Date(date.getFullYear(),date.getMonth()+1,1) - new Date(date);
		var last_day = Math.round(last_day/8.64e7);
		//alert(first_day + "," + last_day);
		
		var skip = true; // 日にちを埋めるかどうかのフラグ
		var thisdate = 1;
		for (var row = 0; row < 6; row++)
		{
			var tr = document.createElement('tr');
			
			for (var col = 0; col < 7; col++){
				if (row === 0 && first_day === col){
					skip = false;
				}
				if (thisdate > last_day) {
					skip = true;
				}
				var td = document.createElement('td');
				td.setAttribute("style","  vertical-align:middle; text-align:center; text-height:40px; font-size:22px; border:1px solid #ddd; border-radius:3px;height:40px; width:40px;white-space:pre;padding:0 auto;");
				if(thisdate == today){
					td.style.color = '#C00';
				}
				td.textContent = skip ? '' : thisdate++;
				if(thisdate<11)td.textContent = ' ' + td.textContent;
				//console.log(td.textContent);
				
				tr.appendChild(td);
			}
			table.appendChild(tr);
		}
		/*calendar.innerHTML = "aa";*/
		calendar.appendChild(table);
	}
	
	
	theme.setContent();
	
	//alert(window.document.getElementsByTagName("content").item(0).tagName);
	//window.document.getElementsByTagName("content").item(0).addEventListener('onresize',"alert('1');");
	//window.document.addEventListener('onresize',"alert('1');");
	//theme.setContent()
}