<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;

 class Historique_Airtel extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("Historique_Airtel_Model");
            ini_set('memory_limit', '1024M'); // Set the memory limit to 256 megabytes
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_airtel", $this->getData());
        }

        public function exporterVersExcel(){
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "historique-airtel-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF", "SOLDE", " ", "TRANSFER_ID", "DATE OPE", "REF", "ACCOUNT_NO", "SENDER MSISDN", "DESTMSISDN", "MONTANT", "DESCRIPTION", "SERVICE_NAME", "REFERENCE_NUMBER", "MONTANT", "SOLDE", "PRINCIPALE"];

            $sheet->mergeCells('A1:J1');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1:J1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('L1:W1');
            $sheet->setCellValue('L1', 'AIRTEL');
            $sheet->getStyle('L1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('L1:W1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('A2:J2');
            $sheet->mergeCells('L2:W2');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A2:J2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('L2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('L1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('E3')->getFont()->setBold(true);

            
            for($i=0; $i < count($cell_array) -3 ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
            }

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            $historique = $this->getData();
            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($historique["historique"] as $dataRow) {
                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow->DATE_OPER);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow->DATE_VAL);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow->DEVISE);
                $sheet->setCellValue($cell_array[3] .$lastRow,$dataRow->MONTANT);
                $sheet->setCellValue($cell_array[4] .$lastRow,$dataRow->LIBELLE);
                $sheet->setCellValue($cell_array[5] .$lastRow,$dataRow->OPER);
                $sheet->setCellValue($cell_array[6] .$lastRow,$dataRow->EXPL);
                $sheet->setCellValue($cell_array[7] .$lastRow,$dataRow->REF_IGOR);
                $sheet->setCellValue($cell_array[8] .$lastRow,$dataRow->solde_boa);
                $sheet->setCellValue($cell_array[10] .$lastRow,$dataRow->TRANSFER_ID);
                $sheet->setCellValue($cell_array[11] .$lastRow,$dataRow->transfer_date);
                $sheet->setCellValue($cell_array[12] .$lastRow,$dataRow->external_id);
                $sheet->setCellValue($cell_array[13] .$lastRow,$dataRow->account_no);
                $sheet->setCellValue($cell_array[14] .$lastRow,$dataRow->sender_msisdn);
                $sheet->setCellValue($cell_array[15] .$lastRow,$dataRow->dest_msisdn);
                $sheet->setCellValue($cell_array[16] .$lastRow,$dataRow->amount);
                $sheet->setCellValue($cell_array[17] .$lastRow,$dataRow->description);
                $sheet->setCellValue($cell_array[18] .$lastRow,$dataRow->service_name);
                $sheet->setCellValue($cell_array[19] .$lastRow,$dataRow->reference_number);
                $sheet->setCellValue($cell_array[20] .$lastRow,$dataRow->solde);
                $sheet->setCellValue($cell_array[21] .$lastRow,$dataRow->solde_airtel);
                $sheet->setCellValue($cell_array[22] .$lastRow,$dataRow->MONTANT + $dataRow->solde);
                $lastRow++;
            }

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');
            // $historique = $this->getData();
            // echo "<pre>";
            //     print_r($historique["historique"][0]->TRANSFER_ID);
            // echo "</pre>";

           
        }

        
        private function getData() {
            $data["historique"] = $this->Historique_Airtel_Model->get_historique();
            return $data;
        }

}



?>