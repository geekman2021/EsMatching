<?php
class Dashboard extends CI_Controller {

    public function __construct() {
        parent::__construct();

        $this->load->model("Historique_Airtel_Model");
        $this->load->model("Historique_Telma_Model");
        $this->load->model("Historique_Orange_Model");
        $this->load->model("Ecart_Telma_Model");
        $this->load->model("Ecart_Airtel_Model");
        $this->load->model("Ecart_Orange_Model");
        ini_set('memory_limit', '2048M');
        session_start();
    }

    public function index() {
        $data['chart_data'] = $this->getDataForDonutChart();
        $this->Historique_Airtel_Model->get_solde_by_month();
        $this->load->view("templates/sidebar");
        $this->load->view('chart-view', $data);
    }

    private function getDataForDonutChart() {

        $airtel = $this->Historique_Airtel_Model->get_historique();
        $orange = $this->Historique_Orange_Model->get_historique();
        $telma = $this->Historique_Telma_Model->get_historique();
        $airtel_line= $this->Historique_Airtel_Model->get_solde_by_month();
        $telma_line= $this->Historique_Telma_Model->get_solde_by_month();
        $orange_line= $this->Historique_Orange_Model->get_solde_by_month();
        $ecart_telma= $this->Ecart_Telma_Model->get_ecart_telma();
        $ecart_boa_telma= $this->Ecart_Telma_Model->get_ecart_boa();
        $ecart_airtel= $this->Ecart_Airtel_Model->get_ecart_airtel();
        $ecart_boa_airtel= $this->Ecart_Airtel_Model->get_ecart_boa();
        $ecart_orange = $this->Ecart_Orange_Model->get_ecart_orange();
        $ecart_boa_orange = $this->Ecart_Orange_Model->get_ecart_boa();

        $solde_telma = 0;
        $solde_boa_telma =0;
        $solde_ecart_telma = 0;
        $solde_ecart_boa_telma = 0;

        $solde_airtel= 0;
        $solde_boa_airtel = 0;
        $solde_ecart_airtel = 0;
        $solde_ecart_boa_airtel = 0;

        $solde_comm= 0;
        $solde_princ = 0;
        $solde_orange = 0;
        $solde_ecart_orange = 0;
        $solde_ecart_princ = 0;
        $solde_ecart_comm= 0;

        foreach($telma as $item) {
            $solde_telma += $item->solde;
            $solde_boa_telma += $item->MONTANT;
        }

        foreach($airtel as $item) {
            $solde_airtel += $item->solde;
            $solde_boa_airtel += $item->MONTANT;
        }

        foreach($orange as $item) {
            $solde_princ += $item->princ_montant;
            $solde_comm += $item->comm_montant;
            $solde_orange += $item->orange_montant;
        }

        foreach($ecart_telma as $item) {
            $solde_ecart_telma += $item->solde;
        }
        foreach($ecart_boa_telma as $item) {
            $solde_ecart_boa_telma += $item->MONTANT;
        }

        foreach($ecart_airtel as $item) {
            $solde_ecart_airtel += $item->solde;
        } 

        foreach($ecart_boa_airtel as $item) {
            $solde_ecart_boa_airtel += $item->MONTANT;
        }

        foreach($ecart_orange as $item) {
            $solde_ecart_orange += $item->orange_montant;
        }
        foreach($ecart_boa_orange as $item) {
            $solde_ecart_princ += $item->princ_montant;
            $solde_ecart_comm += $item->comm_montant;
        }

        $data["solde_ecart_airtel"] = [$solde_ecart_airtel, $solde_ecart_boa_airtel];
        $data["solde_ecart_telma"] = [$solde_ecart_telma, $solde_ecart_boa_telma];
        $data["solde_ecart_orange"] = [$solde_ecart_orange, $solde_ecart_princ, $solde_ecart_comm];
        $data["airtel"] = [$solde_airtel, $solde_boa_airtel] ;
        $data["telma"] = [$solde_telma, $solde_boa_telma];
        $data["orange"] = [$solde_princ, $solde_comm, $solde_orange];

        $data["airtel_line"] = $airtel_line;
        $data["telma_line"] = $telma_line;
        $data["orange_line"] = $orange_line;
        
        return $data;
    }
}
