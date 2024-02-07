<?php
class Dashboard extends CI_Controller {

    public function __construct() {
        parent::__construct();

        $this->load->model("Historique_Airtel_Model");
        $this->load->model("Historique_Telma_Model");
        $this->load->model("Historique_Orange_Model");
        $this->load->model("Ecart_Telma_Model");
        $this->load->model("Ecart_Airtel_Model");
        ini_set('memory_limit', '1024M');
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
        $orange = $this->Historique_Orange_Model->get_last_solde();
        $telma = $this->Historique_Telma_Model->get_last_solde();
        $airtel_line= $this->Historique_Airtel_Model->get_solde_by_month();
        $telma_line= $this->Historique_Telma_Model->get_solde_by_month();
        $ecart_telma= $this->Ecart_Telma_Model->get_ecart_telma();
        $ecart_boa_telma= $this->Ecart_Telma_Model->get_ecart_boa();
        $ecart_airtel= $this->Ecart_Airtel_Model->get_ecart_airtel();
        $ecart_boa_airtel= $this->Ecart_Airtel_Model->get_ecart_boa();

        $data["telma"] = [
            "solde" =>!empty($telma[0]) ? $telma[0] : 0
        ];

        $data["airtel"] = [
            "solde" => !empty($airtel) ? $airtel : 0
        ];

        $data["orange"] = [
            "solde" => !empty($orange[0]) ? $orange[0] : 0
        ];

        $data["airtel_line"] = $airtel_line;
        $data["telma_line"] = $telma_line;
        $data["ecart_telma"] = $ecart_telma;
        $data["ecart_boa_telma"] = $ecart_boa_telma;
        $data["ecart_airtel"] = $ecart_airtel;
        $data["ecart_boa_airtel"] = $ecart_boa_airtel;
        return $data;
    }
}
