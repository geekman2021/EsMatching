<?php 

    class Imprimer extends CI_Controller {
        public function __construct() {
            parent::__construct();
            $this->load->model("check_model");
            $this->load->model("user_model");
            session_start();
        }

        public function index() {
            
        }

        public function imprimerDocument() {
            $i=0;

            $ref_igor = $this->input->get('ref_igor');
            $compte= $this->input->get("compte");
            $montant= $this->input->get("montant");
            $oper= $this->input->get("oper");
            $date= date("d/m/y");
            $annee= date("y");
            $reference= $i++;
            session_destroy();

            $template = $this->load->view('templates/frDix', array(
                'ref_igor' => $ref_igor,
                'compte' => $compte,
                'montant' => $montant,
                'oper' => $oper,
                'date' => $date,
                'annee' => $annee,
                "reference" => $reference

            ));
           
        }

    }


?>