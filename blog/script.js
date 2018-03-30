window.onload = function(){
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
			if(pre.style.maxHeight == ""){
				pre.style.maxHeight = "300px";
			}else{
				pre.style.maxHeight = "";
			}
		};
		if(pre.item(i).offsetHeight > 300){
			div.appendChild(btn);
		}
		console.log(pre.item(i).offsetHeight);
		pre.item(i).style.maxHeight = "300px";
	}
	}
};