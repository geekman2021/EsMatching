<?php

require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\Spreadsheet;
 class Historique_Orange extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("Historique_Orange_Model");
            ini_set('memory_limit', '1024M');
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/historique/historique");
            $this->load->view("pages/historique/historique_orange", $this->get_data());

        }

        private function get_data() {
            $data["historique"] = $this->Historique_Orange_Model->get_historique();
            return $data; 
        }

        public function exporterVersExcel() {
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "HistoriqueOrange-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF IGOR", "REF_REL", "PHONE","SOLDE", "", "DATE_OPER", "DATE_VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE", "EXPL", "REF_IGOR", "OPER_ID", "CODE AGENCE","SOLDE", "", "DATE", "HEURE", "REFERENCE", "SERVICE", "NUM_COMPTE", "DEBIT", "CREDIT", "MONTANT", "SOLDE", "DEBIT", "CREDIT", "MONTANT", "SOLDE", "PRINCIPALE", "COMM", "PRINCIPAL", "COMM"];



            $sheet->mergeCells('A1:W1');
            $sheet->getStyle('A1:W1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1')->getFont()->setBold(true);

            $sheet->mergeCells('A2:K2');
            $sheet->getStyle('A2:J2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            
            $sheet->mergeCells('L2:W2');
            $sheet->getStyle('L2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('L2', 'COMMISSION');
            $sheet->getStyle('L2')->getFont()->setBold(true);
            $sheet->getStyle('L2')->getAlignment()->setHorizontal('center');

            $sheet->mergeCells('Y1:AK2');
            $sheet->getStyle('Y1:AK2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->setCellValue('Y1', 'ORANGE');
            $sheet->getStyle('Y1')->getFont()->setBold(true);
            $sheet->getStyle('Y1')->getAlignment()->setHorizontal('center');

            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('M2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('M2:AH2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('M1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('M2')->getFont()->setBold(true);

            for($i=0; $i < count($cell_array) -3 ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
            }

            // EN TETE 

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."3", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "3")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "3")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            $historique = $this->get_data();
            $lastRow = $sheet->getHighestRow() + 1;
            foreach($historique["historique"] as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_date_oper);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_date_val);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_devise);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_montant); 
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_libelle);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_oper);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_expl);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_ref_igor);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_ref_rel);
                $sheet->setCellValue($cell_array[$i++].$lastRow, substr($dataRow->cle, 0, 10));
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->princ_montant);
                $i+=1;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow->comm_date_oper);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_date_val);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_devise);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_montant);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_libelle);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_oper);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_expl);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_ref_igor);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_ref_rel);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_code_agence);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_montant);
                $i+=1;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_date);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_heure);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_ref);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_service);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_num_compte);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_debit);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_credit);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_montant);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_solde);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_sous_distri);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_super_distri);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->orange_super_distri);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->solde);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->princ_montant + $dataRow->orange_montant);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow->comm_montant + $dataRow->orange_super_distri);

                $lastRow++;
            }


            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');
        }
 }



?>