
window.onload = function(){
	/* 給所有的代碼加入 select 和 roll */
	var pre = document.getElementsByTagName("pre");
	for(var i=0;i<pre.length;i++){
	if(pre.item(i).firstChild.tagName != undefined && pre.item(i).firstChild.tagName.toLowerCase() == "code"){
		var div = document.createElement("div");
		pre.item(i).parentNode.insertBefore(div,pre.item(i));
		div.classList.add('code-control-div');
		
		var btn = document.createElement("A");
		btn.innerHTML = "Select";
		btn.classList.add('code-control-button');
		btn.onclick = function() {
			var range = document.createRange();
			var referenceNode = this.parentNode.nextSibling.firstChild;
			range.selectNode(referenceNode);
			window.getSelection().removeAllRanges();
			window.getSelection().addRange(range);
		};
		div.appendChild(btn);
		
		var btn = document.createElement("A");
		btn.innerHTML = "roll";
		btn.classList.add('code-control-button');
		btn.onclick = function() {
			var pre = this.parentNode.nextSibling;
			console.log(pre.style.maxHeight);
			if(pre.firstChild.style.maxHeight == ""){
				pre.firstChild.style.maxHeight = "200px";
			}else{
				pre.firstChild.style.maxHeight = "";
			}
		};
		if(pre.item(i).offsetHeight > 300){
			div.appendChild(btn);
		}
		console.log(pre.item(i).offsetHeight);
		pre.item(i).firstChild.style.maxHeight = "200px";
		console.log(pre.item(i));
	}
	}
	/* 圖片放大和窗口適應 */
	var imgs = document.getElementsByTagName("img");
	var windowWidth = document.body.clientWidth*0.5-5;
	for(var i=0;i<imgs.length;i++){
		/*WWWW = 560;
		if(imgs.item(i).naturalWidth < WWWW){
			x = windowWidth;
			console.log(imgs.item(i).natrulWidth);
			//imgs.item(i).style.width =x + "px";
			imgs.item(i).style.width = "calc(50% - 5px)";
			imgs.item(i).style.cssFloat = "left";
			imgs.item(i).style.marginRight = "5px";
			imgs.item(i).style.marginBottom = "5px";
		}
		if(imgs.item(i).naturalWidth > document.body.clientWidth*0.5){
			//imgs.item(i).style.minWidth = "50%";
			//imgs.item(i).style.maxWidth = "100%";
		}
		*/

		var theImg = imgs.item(i);

		//imgs.item(i).parentNode.removeChild(

		imgs.item(i).onclick = function(x){
			var e = x.target;		
			div = document.createElement("div");
			document.body.insertBefore(div,document.body.firstChild);
			div.classList.add('full-screen-img-watch');
			
			img = document.createElement("IMG");
			img.src = e.src;
			//img.classList.add('full-screen-img-watch');
			
			div.appendChild(img);
			
			div.onclick = function(e){
				this.parentNode.removeChild(this);
			}
		}
		
		
	}
};