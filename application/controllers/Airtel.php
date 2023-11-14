<?php

    defined('BASEPATH') OR exit('No direct script access allowed');
    // require "vendor/autoload.php";
    

    class Airtel extends CI_Controller {
        public function __construct(){
            parent::__construct();
            session_start();
            $this->load->model("Airtel_Normal_Model");
            $this->load->model("Airtel_Anomalie_Model");
            $this->load->model("Igor_Airtel_Anomalie_Model");
            $this->load->model("Igor_Airtel_Model");


        }
        public function index() {
            
            $this->load->view("templates/sidebar");

            $data["anomalie_ci"] = $this->Airtel_Anomalie_Model->get_anomalie_ci();
            $data["anomalie_co"] = $this->Airtel_Anomalie_Model->get_anomalie_co();
            $data["igor_anomalie_ci"]= $this->Igor_Airtel_Anomalie_Model->get_anomalie_ci();
            $data["igor_anomalie_co"]= $this->Igor_Airtel_Anomalie_Model->get_anomalie_co();
            $data["deallocation"] = $this->Airtel_Anomalie_Model->get_deallocation();
            $data["ambiguous"] = $this->Airtel_Anomalie_Model->get_ambiguous();
            $data["ambiguousCO"] = $this->Airtel_Anomalie_Model->get_ambiguousCO();
            $data["success_no_refCO"] = $this->Airtel_Anomalie_Model->get_SuccessNoReferenceCO();
            $data["success_no_refCI"] = $this->Airtel_Anomalie_Model->get_SuccessNoReferenceCI();
            $data["normale_ci"]= $this->Igor_Airtel_Model->get_ci();
            $data["normale_co"] = $this->Igor_Airtel_Model->get_co();
            $data["vi"] = $this->Igor_Airtel_Anomalie_Model->get_vi();
            $this->load->view("pages/operateur/airtel-list", $data);
        }

        // public function regularisation 

    

        public function edit_anomalie_CO() {

            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->Airtel_Anomalie_Model->update_anomalie_co($id, $etat, $date_regul);

        }

        public function edit_anomalie_CI() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->Airtel_Anomalie_Model->update_anomalie_CI($id, $etat, $date_regul);

        }

        public function edit_anomalie_deallo() {
            $id= $this->input->post("id");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->Airtel_Anomalie_Model->update_anomalie_deallo($id, $etat, $date_regul);
    
        }

        public function edit_anomalie_igor_ci() {
            $ref_igor= $this->input->post("ref_igor");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");

            $this->igor_Airtel_Anomalie_Model->update_anomalie_ci($ref_igor, $etat, $date_regul);

        }


        public function edit_anomalie_igor_co() {
            $ref_igor= $this->input->post("ref_igor");
            $etat= $this->input->post("etat");
            $date_regul= $this->input->post("date_regul");
            $this->igor_Airtel_Anomalie_Model->update_anomalie_co($ref_igor, $etat, $date_regul);

        }
    }


    ?>



