export default {
    name: 'SelectInput',
    components: {},
    data: function() {
        return {
            count: "File"
        }
    },
    props: ['value', 'selected'],
    methods: {
        toggle() {

            this.$emit('input', !this.value)
            if (this.value) {
                this.$datas.selected_input = "Camera";
                this.count = "Camera";

            } else {
                this.$datas.selected_input = "File";
                this.count = "File";
            }
        }
    },
    computed: {
        backgroundStyles() {
            return {
                backgroundColor: this.value ? '#3490dc' : '#dae1e7'
            }
        },
        indicatorStyles() {
            return { transform: this.value ? 'translateX(2rem)' : 'translateX(0)' }
        }
    },
    style: `
    .toggle-wrapper {
        display: inline-block;
        position: relative;
        cursor: pointer;
        height: 2rem;
        width: 4rem;
        border-radius: 9999px;
    }

    .toggle-wrapper:focus {
        outline: 0;
        box-shadow: 0 0 0 4px rgba(52, 144, 220, .5);
    }

    .toggle-background {
        display: inline-block;
        border-radius: 9999px;
        height: 100%;
        width: 100%;
        box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.1);
        transition: background-color .2s ease;
    }

    .toggle-indicator {
        position: absolute;
        top: .25rem;
        left: .25rem;
        height: 1.5rem;
        width: 1.5rem;
        background-color: #fff;
        border-radius: 9999px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        transition: transform .2s ease;
    }
    `,
    template: `
    <div style="display: flex;padding-top: 8px;padding-bottom: 8px;">
        <h3 style="padding-right: 10px;">Input:</h3>
        <span class="toggle-wrapper" @click="toggle" role="checkbox" :aria-checked="value.toString()" tabindex="0" @keydown.space.prevent="toggle">
            <span class="toggle-background" :style="backgroundStyles"></span>
            <span class="toggle-indicator" :style="indicatorStyles"></span>
        </span>
        <h4 style="padding-top: 5px;padding-left: 10px;">{{count}}</h4>
    </div>
  `,
};
