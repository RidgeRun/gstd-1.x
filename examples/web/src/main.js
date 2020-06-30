import controller_app from './components/controller_app.js';
import video_player_app from './components/video_player_app.js';

if (document.getElementById("video_player")) {

    new Vue({
        render: h => h(video_player_app),
    }).$mount(`#video_player`);
}

if (document.getElementById("controller")) {

    new Vue({
        render: h => h(controller_app),
    }).$mount(`#controller`);

}