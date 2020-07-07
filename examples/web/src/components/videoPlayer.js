/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

export default {
    name: 'VideoPlayer',
    components: {},
    data() {
        return {
            componentKey: 0,
            text: '',
            userId: 0,
            placeholder: "Pipeline"
        }
    },
    methods: {
        forceRerender() {
            this.componentKey += 1;
        },
        updatePaused(event) {
            this.$datas.videoElement = event.target;
            this.$datas.paused = event.target.paused;
        },
        play: async function() {
            try {
                await this.$datas.gstc.pipeline_play(this.$datas.pipeName);
            } catch (error) {
                this.$root.$emit("console_write", "Play", error);
                return;
            }
        },
        pause: async function() {
            try {
                await this.$datas.gstc.pipeline_pause(this.$datas.pipeName);
            } catch (error) {
                this.$root.$emit("console_write", "Pause", error);
                return;
            }
        },
        create_pipeline: async function(event) {
            if (this.config == true) {
                var pipe = this.text;
            } else {
                if (this.$datas.selectedInput == "File") {
                    var pipe = "filesrc location=" + this.text + " ! decodebin ! videoconvert ! autovideosink";
                } else {
                    var pipe = "v4l2src device=" + this.text + " ! videoconvert ! textoverlay text=\"Room A\" valignment=top halignment=left font-desc=\"Sans, 72 \" ! autovideosink";
                }
            }
            try {
                await this.$datas.gstc.pipeline_create(this.$datas.pipeName, pipe);
                var res = await this.$datas.gstc.pipeline_play(this.$datas.pipeName);
                this.forceRerender();
                var res = await this.$datas.gstc.pipeline_pause(this.$datas.pipeName);
                this.$root.$emit('myEvent', 'new message!');
                this.$root.$emit('busevent', 'new message!');
                this.$datas.busEnable = true;
            } catch (error) {
                this.$root.$emit("console_write", "Create", error);
                return;
            }
        },
        delete_pipeline: async function(event) {
            try {
                await this.$datas.gstc.pipeline_delete(this.$datas.pipeName);
            } catch (error) {
                this.$root.$emit("console_write", "Delete", error);
                return;
            }
        },
        stop_video: async function(event) {
            try {
                await this.$datas.gstc.pipeline_stop(this.$datas.pipeName);
                if (this.config == false) {
                    await this.$datas.gstc.pipeline_delete(this.$datas.pipeName);
                }
            } catch (error) {
                this.$root.$emit("console_write", "Stop", error);
                return;
            }
        },
        speed_video: async function(speed) {
            try {
                var actual_position = await this.$datas.gstc.read("/pipelines/"+this.$datas.pipeName+"/position")
                this.$datas.speed = speed;
                if (this.$datas.direction == 1) {
                    var res = await this.$datas.gstc.event_seek(this.$datas.pipeName, speed, 3, 1, 1, actual_position.response.value, 1, -1)
                } else {
                    var res = await this.$datas.gstc.event_seek(this.$datas.pipeName, -speed, 3, 1, 1, 0, 1, actual_position.response.value);
                }

            } catch (error) {
                this.$root.$emit("console_write", "Speed", error);
                return;
            }

        },
        speed_direction: async function(direction) {
            this.$datas.direction = direction;
            this.speed_video(this.$datas.speed);
        },
        jump_to: async function(seconds) {
            try {
                await this.$datas.gstc.event_seek(this.$datas.pipeName, this.$datas.speed, 3, 1, 1, this.userId * 1000000000, 1, -1)
            } catch (error) {
                this.$root.$emit("console_write", "Jump to", error);
                return;
            }
        }
    },
    created: function() {
        if (this.config == true) {
            this.placeholder = "Pipeline";
        } else {
            this.placeholder = "File or camera path";
        }
    },
    computed: {
        playing() { return !this.$datas.paused; }
    },
    props: ['name', 'enable', 'config'],

    template: `
    <div> 
    <div style="display:flex;height: 40px;">
        <b-form-input v-model="text" :placeholder="placeholder"></b-form-input>    
        <b-button v-on:click="create_pipeline()">
            <b-icon icon="box-arrow-up-right"></b-icon>
        </b-button>
        <b-button v-on:click="delete_pipeline()">
        <b-icon icon="trash"></b-icon>
    </b-button>
    </div>

    <b-button-group class="mx-1">
        <b-button  v-on:click="play()">
            <b-icon icon="play-fill"></b-icon>
        </b-button>
        <b-button  v-on:click="pause()">
            <b-icon icon="pause-fill"></b-icon>
        </b-button>
        <b-button v-on:click="stop_video()">
            <b-icon icon="stop-fill"></b-icon>
        </b-button>
    </b-button-group>

    <b-dropdown :disabled="!enable" class="mx-1" right text="Speed">
        <b-dropdown-item v-on:click="speed_video(0.25)">0.25</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(0.50)">0.50</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(0.75)">0.75</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(1.00)">normal</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(1.25)"> 1.25</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(1.50)">1.50</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(1.75)">1.75</b-dropdown-item>
        <b-dropdown-item v-on:click="speed_video(2.00)"> 2.00</b-dropdown-item>
    </b-dropdown>
    <b-button-group  class="mx-1">
    <b-button :disabled="!enable"  v-on:click="speed_direction(0)">
        <b-icon icon="chevron-left"></b-icon>
    </b-button>
    <b-button :disabled="!enable" v-on:click="speed_direction(1)">
        <b-icon icon="chevron-right"></b-icon>
    </b-button>
</b-button-group>
    <div style="display:ruby;padding-right: 20px;height: 40px;">
        <b-form-group label-size="md" label-cols-sm="3" label="Jump to" label-align-lg="right" label-for="nested-street">
            <b-form-input :disabled="!enable" v-model="userId" id="nested-street" size="md"></b-form-input>
        </b-form-group>
        <b-button :disabled="!enable" v-on:click="jump_to(15000000000)">
            <b-icon icon="box-arrow-up-right"></b-icon>
        </b-button>
    </div>

</div>
  `,
};
