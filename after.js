// 點擊 H2 或 H3 標題可以隱藏其子內容
$("article h2").click(function () {
    $(this).nextUntil("article h2").slideToggle(300);

});
$("article h3").click(function () {
    $(this).nextUntil("article h2,article h3").slideToggle(300);
});

// 點圖片在新窗口中打開
$("img").click(function(){
    window.open($(this).prop("src"));
});

// 判斷是否是寬屏模式，如果是，顯示 QRCODE，目錄和時間
if($("aside").css("display") == "block") {
    // 加載 QRCODE 庫，加載完成後顯示當前頁面的 QRCODE
    $.getScript("qrcode.js",function () {
        var qrcode = new QRCode("qrcode", {
            width: 200,
            height: 200,
            colorDark: "#000000",
            colorLight: "#ffffff",
            correctLevel: QRCode.CorrectLevel.H
        });
        $("#qrcode").hide()
        qrcode.makeCode($(location).attr('href'));
        //$("#qrcode").hide();
        $("#qrcode").slideDown(300);
    });
    // 自動生成目錄
    $("article").children().each(function(){
        var theTag = $(this);
        if(theTag.prop("tagName").toLowerCase() == "h1" ||
            theTag.prop("tagName").toLowerCase() == "h2" ||
            theTag.prop("tagName").toLowerCase() == "h3") {
            var theEle = $("<li><" + theTag.prop("tagName") + ">" +
                theTag.text() + "</" + theTag.prop("tagName") + "aaa" + "</li>");
            // 加入書籤
            $("#catalog ul").append(theEle);
            // 點擊後跳轉位置
            theEle.click(function () {
                var theAnchor = $("#" + theTag.text().replace(/ /g,"_"));
                // 一個效果，提示當前標題
                $('html, body').animate({scrollTop: theAnchor.offset().top}, 300,function(){
                    theAnchor.fadeTo(10,0.15,function(){
                        theAnchor.fadeTo(300,1);
                    });
                });
            });
            // 給標籤加入 ID，用以導航
            theTag.prop("id",theTag.text().replace(/ /g,"_"));
        }
    });
    // 目錄跟隨窗口
    $(window).scroll(function() {
        var theCatalog = $("#catalog");
        var thePrev = theCatalog.prev();
        // 判斷窗口移動的位置，更新 CSS
        if( $(window).scrollTop() > thePrev.position().top + thePrev.height()) {
            theCatalog.css({
                position: 'fixed',
                top: '0px',
                left: $("body").left,
                width: theCatalog.width(),
                /*height: $('#catalog').height(),*/
                backgroundColor: "white",
            });
        } else {
            theCatalog.css({
                position: 'static'
            });
        }
    });
    // 在側邊欄加入日期
    //$("aside").append("<p>" + Date() + "</p>");
    //$("aside").append("<p>" + $(location).attr('href') + "</p>");
}else {
    // 添加隱藏導航欄的機能
    $("#hideNav").click(function(){
        $(this).parent().children().not(this).slideToggle(300);
    });
    $("footer").children().eq(0).append(" | smart phone version");
}
/*******************************************************************************
 Debug Only
 *******************************************************************************/
$("#debug").click(function(){
    var str = $(location).attr('href');
    str = str.substr(str.indexOf("?")+1);
    $("article").load(str);
})

