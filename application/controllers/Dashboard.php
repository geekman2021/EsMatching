<?php
class Dashboard extends CI_Controller {

    public function index() {
        $data['chart_data'] = $this->getDataForDonutChart();
        $this->load->view("templates/sidebar");

        $this->load->view('chart-view', $data);
    }

    private function getDataForDonutChart() {

        $data = [
            'labels' => ['Rouge', 'Bleu', 'Vert'],
            'datasets' => [
                [
                    'data' => [30, 40, 20],
                    'backgroundColor' => ['rgb(255, 99, 132)', 'rgb(54, 162, 235)', 'rgb(75, 192, 192)'],
                ],
            ],
        ];
        return $data;
    }
}
