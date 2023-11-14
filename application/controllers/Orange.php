<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Orange extends CI_Controller {
        public function __construct(){
            parent::__construct();
            $this->load->model("Orange_Normal_Model");
            $this->load->model("Orange_Anomalie_Model");
            $this->load->model("Boa_Orange_Anomalie_Model");
            session_start();
        }
        public function index() {
            
            $this->load->view("templates/sidebar");
            $this->load->view("pages/operateur/orange-list", $this->get_data());
        }
        

        public function get_data() {

            $data["normal_ci"] = $this->load->Orange_Normal_Model->get_ci();
            $data["normal_co"] = $this->load->Orange_Normal_Model->get_co();
            $data["anomalie_co"] = $this->load->Orange_Anomalie_Model->get_co();
            $data["anomalie_ci"] = $this->load->Orange_Anomalie_Model->get_ci();
            $data["ind"] = $this->load->Orange_Anomalie_Model->get_ind();
            $data["dat"] = $this->load->Boa_Orange_Anomalie_Model->get_ci();
            $data["cat"] = $this->load->Boa_Orange_Anomalie_Model->get_co();
            $data["vi"] = $this->load->Boa_Orange_Anomalie_Model->get_vi();

            return $data;

        }
    }


    ?>



