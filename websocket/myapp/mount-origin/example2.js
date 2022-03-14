var websocket = null;
//将消息显示在网页上
function setMessageInnerHTML(innerHTML) {
    document.getElementById('message').innerHTML += innerHTML + '<br/>';
}

//关闭连接
function closeWebSocket() {
    websocket.close();
}


function get_appropriate_ws_url(extra_url)
{
	var pcol;
	var u = document.URL;

	/*
	 * We open the websocket encrypted if this page came on an
	 * https:// url itself, otherwise unencrypted
	 */

	if (u.substring(0, 5) === "https") {
		pcol = "wss://";
		u = u.substr(8);
	} else {
		pcol = "ws://";
		if (u.substring(0, 4) === "http")
			u = u.substr(7);
	}

	u = u.split("/");

	/* + "/xxx" bit is for IE10 workaround */

	return pcol + u[0] + "/" + extra_url;
}
function connect() {
    //判断当前浏览器是否支持WebSocket, 主要此处要更换为自己的地址
    if ('WebSocket' in window) {

        // ws_url = `ws://${input}`
        ws_url = get_appropriate_ws_url("");
        console.log("ws_url: ",ws_url);
        // websocket = new WebSocket("ws://localhost:3000");
        websocket = new WebSocket(ws_url);

        //连接发生错误的回调方法
        websocket.onerror = function () {
            setMessageInnerHTML("error");
        };

        //连接成功建立的回调方法
        websocket.onopen = function (event) {
            //setMessageInnerHTML("open");
        }

        //接收到消息的回调方法
        websocket.onmessage = function (event) {
            setMessageInnerHTML(event.data);
        }

        //连接关闭的回调方法
        websocket.onclose = function () {
            setMessageInnerHTML("close");
        }

        //监听窗口关闭事件，当窗口关闭时，主动去关闭websocket连接，防止连接还没断开就关闭窗口，server端会抛异常。
        window.onbeforeunload = function () {
            websocket.close();
        }
    } else {
        alert('Not support websocket')
    }
}
window.onload=function(){
    btn = document.getElementById("send_btn");
    connect();

    btn.onclick = function(e){
        msg = document.getElementById("text").value;

        websocket.send(msg);
        console.log("ws send: ",msg)
    }

}