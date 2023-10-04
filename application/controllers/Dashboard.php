<?php
class Dashboard extends CI_Controller {

    public function __construct() {
        parent::__construct();

        $this->load->model("historique_airtel_model");
        $this->load->model("historique_telma_model");
        $this->load->model("historique_orange_model");
    }

    public function index() {
        $data['chart_data'] = $this->getDataForDonutChart();
        $this->load->view("templates/sidebar");
        $this->load->view('chart-view', $data);
    }

    private function getDataForDonutChart() {
        $airtel = $this->historique_airtel_model->get_last_solde();
        // $orange = $this->historique_orange_model->get_last_solde();
        $telma = $this->historique_telma_model->get_last_solde();

        $data["telma"] = [
            "solde" =>$telma[0]
        ];

        $data["airtel"] = [
            "solde" => $airtel[0]
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
