export default {
    name: 'FooterControl',
    components: {},
    data: function() {
        return {
            count: 0
        }
    },
    props: ['name'],
    template: `
  <b-navbar style="" type="light" variant="light">
    <b-button size="sm" class="my-2 my-sm-0"  variant="light" href="https://www.ridgerun.com/" target="_blank"><img src="./resources/logo.png" alt="Ridgerun" ></b-button>
    <b-navbar-nav class="ml-auto">
      <b-button size="lg" class="my-2 my-sm-0"  variant="info" href="https://www.ridgerun.com/contact" target="_blank"> Contact us</b-button>
    </b-navbar-nav>
  
</b-navbar>
`,
};