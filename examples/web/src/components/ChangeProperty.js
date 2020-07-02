export default {
    name: 'ChangeProperty',
    components: {},
    data() {
        return {
            text: ''
        }
    },
    methods: {
        create_pipeline: async function(event) {
            var res = await this.name.element_set("p0", this.$datas.selected_element, this.$datas.selected_properties, this.text)
        }
    },
    props: ['name'],
    template: `
    <div style="display:contents;height: 40px;">
        <div style="padding-top: 10px;">
        <label>New value</label>
        </div>
        
        <div style="display:inline-flex;">
        <b-form-input v-model="text" placeholder="New value"></b-form-input>    
        <b-button v-on:click="create_pipeline()">
            <b-icon icon="box-arrow-up-right"></b-icon>
        </b-button>
        </div>
        
    </div>
`
};
