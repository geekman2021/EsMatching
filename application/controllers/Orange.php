<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Orange extends CI_Controller {
        public function __construct(){
            parent::__construct();
            $this->load->model("orange_normal_model");
            $this->load->model("orange_anomalie_model");
            $this->load->model("boa_orange_anomalie_model");
        }
        public function index() {
            
            $this->load->view("templates/sidebar");
            $this->load->view("pages/operateur/orange-list", $this->get_data());
        }
        

        public function get_data() {

            $data["normal_ci"] = $this->load->orange_normal_model->get_ci();
            $data["normal_co"] = $this->load->orange_normal_model->get_co();
            $data["anomalie_co"] = $this->load->orange_anomalie_model->get_co();
            $data["anomalie_ci"] = $this->load->orange_anomalie_model->get_ci();
            $data["ind"] = $this->load->orange_anomalie_model->get_ind();
            $data["dat"] = $this->load->boa_orange_anomalie_model->get_ci();
            $data["cat"] = $this->load->boa_orange_anomalie_model->get_co();
            $data["vi"] = $this->load->boa_orange_anomalie_model->get_vi();

            return $data;

        }
    }


    ?>



