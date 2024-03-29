<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;
 class Historique_Telma extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("Historique_Telma_Model");
            ini_set('memory_limit', '2048M'); // Set the memory limit to 256 megabytes
            session_start();

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_telma", $this->get_data());
        }

        public function get_data() {
            $data["historique"] = $this->Historique_Telma_Model->get_historique();

            return $data;
            
        }

        public function exporterVersExcel(){
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "Historique-Rappro" .$dateAujourdhui .".xlsx";

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

            $historique = $this->get_data();

            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($historique["historique"] as $dataRow) {
                
                $sheet->setCellValue($cell_array[0] .$lastRow, $dataRow->DATE_OPER);
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow->DATE_VAL);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow->DEVISE);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow->MONTANT);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow->LIBELLE);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow->OPER);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow->EXPL);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow->REF_IGOR);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow->cle);
                $sheet->setCellValue($cell_array[9] .$lastRow, $dataRow->solde_boa);
                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow->date_d);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow->trans_id);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow->initiator);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow->TYPE);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow->channel);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow->state);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow->wallet);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow->Amount_MGA);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow->sender);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow->sender_name);
                $sheet->setCellValue($cell_array[22] .$lastRow, $dataRow->receiver);
                $sheet->setCellValue($cell_array[23] .$lastRow, $dataRow->receiver_name);
                $sheet->setCellValue($cell_array[24] .$lastRow, $dataRow->details1);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow->confirming_agent);
                $sheet->setCellValue($cell_array[25] .$lastRow, $dataRow->notify_alternate);
                $sheet->setCellValue($cell_array[26] .$lastRow, $dataRow->origine);
                $sheet->setCellValue($cell_array[27] .$lastRow, $dataRow->TVA);
                $sheet->setCellValue($cell_array[29] .$lastRow, $dataRow->ACTION);
                $sheet->setCellValue($cell_array[30] .$lastRow, $dataRow->AA1_GROUP);
                $sheet->setCellValue($cell_array[31] .$lastRow, $dataRow->PAR);
                $sheet->setCellValue($cell_array[32] .$lastRow, $dataRow->solde);
                $sheet->setCellValue($cell_array[33] .$lastRow, $dataRow->solde_telma);
                $sheet->setCellValue($cell_array[34] .$lastRow, $dataRow->MONTANT - $dataRow->Amount_MGA);
                
                
                $lastRow++;
            }
            
            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');
            $historique = $this->getData();
            // echo "<pre>";
            //     print_r($historique["historique"][0]->TRANSFER_ID);
            // echo "</pre>";

           
        }

 }



?>