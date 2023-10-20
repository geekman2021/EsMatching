<?php
    class Regul_Airtel extends CI_Controller {

        public function __construct() {
            parent::__construct();
            $this->load->model("regul_airtel_model");
            
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/regul/regul");
            $this->load->view("pages/regul/regul_airtel", $this->get_data());
        }

        public function get_data() {

            $data["ci"] = $this->regul_airtel_model->get_regul_ci();
            $data["ambi"] = $this->regul_airtel_model->get_ambi();
            $data["deallo_vi"] = $this->regul_airtel_model->get_deallo_vi();
            return $data;
        }
        
    }
    



?>