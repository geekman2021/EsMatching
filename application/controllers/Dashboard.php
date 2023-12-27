<?php
class Dashboard extends CI_Controller {

    public function __construct() {
        parent::__construct();

        $this->load->model("Historique_Airtel_Model");
        $this->load->model("Historique_Telma_Model");
        $this->load->model("Historique_Orange_Model");
        session_start();
    }

    public function index() {
        $data['chart_data'] = $this->getDataForDonutChart();
        $this->load->view("templates/sidebar");
        $this->load->view('chart-view', $data);
        
    }

    private function getDataForDonutChart() {

        $airtel = $this->Historique_Airtel_Model->get_last_solde();
        $orange = $this->Historique_Orange_Model->get_last_solde();
        $telma = $this->Historique_Telma_Model->get_last_solde();

        $data["telma"] = [
            "solde" =>!empty($telma[0]) ? $telma[0] : 0
        ];

        $data["airtel"] = [
            "solde" => !empty($airtel[0]) ? $airtel[0] : 0
        ];

        $data["orange"] = [
            "solde" => !empty($orange[0]) ? $orange[0] : 0
        ];

        // $data = [
        //     'labels' => ['Solde BOA', 'Solde Telma', 'Vert'],
        //     'datasets' => [
        //         [
        //             'data' => [30, 40, 20],
        //             'backgroundColor' => ['rgb(255, 99, 132)', 'rgb(54, 162, 235)', 'rgb(75, 192, 192)'],
        //         ],
        //     ],
        //     "airtel" => $airtel,
        //     // "orange" => [$orange],
        //     'telma' => $telma
        // ];
        return $data;
    }
}
