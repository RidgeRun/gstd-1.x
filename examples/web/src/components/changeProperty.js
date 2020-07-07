/*
 * Created by RidgeRun, 2020
 *
 * The software contained in this file is free and unencumbered software
 * released into the public domain. Anyone is free to use the software
 * contained in this file as they choose, including incorporating it into
 * proprietary software.
 */

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
            var res = await this.$datas.gstc.element_set(this.$datas.pipeName, this.$datas.selectedElement, this.$datas.selectedProperties, this.text)
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
