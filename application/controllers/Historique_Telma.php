<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;
 class Historique_Telma extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("historique_telma_model");

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_telma", $this->get_data());
        }

        public function get_data() {
            $data["historique"] = $this->historique_telma_model->get_historique();

            return $data;
            
        }

        public function exporterVersExcel(){
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproTelma-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF", "", "REF2", "SOLDE", "", "date_d", "trans_id", "initiator", "TYPE", "Channel", "State", "Wallet", "Amount_MGA", "Sender", "Sender_name", "receiver", "receiver_name", "details1", "Confirming_agent", "notify_alternate", "Origine", "TVA", "ACTION", "AA1 GROUP", "PAR", "MONTANT", "SOLDE", "ECART", "SOLDE DISPO", "SOLDE TELMA SALFECARE"];

            $sheet->mergeCells('A1:K1');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1:K1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('M1:AH1');
            $sheet->setCellValue('M1', 'Telma');
            $sheet->getStyle('M1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('M1:AH1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('A2:K2');
            $sheet->mergeCells('M2:AH2');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A2:K2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('M2:AH2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('M1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('E3')->getFont()->setBold(true);

            
            //  BORDURE 
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
                $sheet->setCellValue($cell_array[22] .$lastRow,$dataRow->MONTANT - $dataRow->solde);
                $lastRow++;
            }
            
            // NORMALE CI 
            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');
            // $historique = $this->getData();
            // echo "<pre>";
            //     print_r($historique["historique"][0]->TRANSFER_ID);
            // echo "</pre>";

           
        }

 }



?>