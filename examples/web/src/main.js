/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

import controllerApp from './components/controllerApp.js';
import videoPlayerApp from './components/videoPlayerApp.js';

if (document.getElementById("videoPlayer")) {

    new Vue({
        render: h => h(videoPlayerApp),
    }).$mount(`#videoPlayer`);
}

if (document.getElementById("controller")) {

    new Vue({
        render: h => h(controllerApp),
    }).$mount(`#controller`);
}
