<?php

    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;

    class Importer_Orange extends CI_Controller {
       
        public function __construct(){
            parent::__construct();
            $this->load->model("boa_orange_anomalie_model");
            $this->load->model("orange_normal_model");
            $this->load->model("orange_anomalie_model");
            $this->load->model("historique_orange_model");
            
        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/orange-form");
            
        }

        public function importer() {
            
            $orange_read = $this->readData($_FILES["orange"]["name"], $_FILES["orange"]["tmp_name"]);
            $princ_read = $this->readData($_FILES["princ"]["name"], $_FILES["princ"]["tmp_name"]);
            $com_read= $this->readData($_FILES["com"]["name"], $_FILES["com"]["tmp_name"]);

            if (count($orange_read) > 1 ) {
                
                for($i=1; $i < count($orange_read); $i++) {

                    $debit= str_replace([" ", ","], "", $orange_read[$i][6]);
                    $credit= str_replace([" ", ","], "", $orange_read[$i][7]);
                    $cle= $orange_read[$i][5] .$debit .$credit ;
                    $super_distri= str_replace([" ", ".0"], "", $orange_read[$i][8]); 
                    $sous_distr= str_replace([" ", ".0"], "", $orange_read[$i][9]); 

                    // $debit = floatval($debit);
                    // $debit = intval($debit);
                    $orange[]= array(
                       "orange_num" => $orange_read[$i][0],
                       "orange_date" =>  $orange_read[$i][1],
                       "orange_heure" => $orange_read[$i][2],
                       "orange_ref" => $orange_read[$i][3],
                       "orange_service" => $orange_read[$i][4],
                       "orange_num_compte" => $orange_read[$i][5],
                       "orange_debit" => $debit,
                       "orange_credit" => $credit,
                       "orange_super_distri" => $super_distri,
                       "orange_sous_distri" => $sous_distr,
                       "cle" => $cle
                    );
                }
            }

            if (count($princ_read) > 1 ) {
                
                for($i=1; $i < count($princ_read); $i++) {

                    $montant = strval($princ_read[$i][4]);
                    $montant = str_replace(".00", "" , $montant);
                    $princ[]= array(
                       "princ_compte" => $princ_read[$i][0],
                       "princ_date_oper" =>  $princ_read[$i][1],
                       "princ_date_val" => $princ_read[$i][2],
                       "princ_devise" => $princ_read[$i][3],
                       "princ_montant" => $montant,
                       "princ_libelle" => $princ_read[$i][5],
                       "princ_oper" => $princ_read[$i][6],
                       "princ_expl" => $princ_read[$i][7],
                       "princ_ref_igor" => $princ_read[$i][8],
                       "princ_ref_rel" => $princ_read[$i][9],
                    );
                }
            }

            if (count($com_read) > 1 ) {
                
                for($i=1; $i < count($com_read); $i++) {

                    $codeAgence= $com_read[$i][9];
                    $codeAgence= explode("-", $codeAgence);
                    $comm[]= array(
                       "comm_compte" => $com_read[$i][0],
                       "comm_date_oper" =>  $com_read[$i][1],
                       "comm_date_val" => $com_read[$i][2],
                       "comm_devise" => $com_read[$i][3],
                       "comm_montant" => $com_read[$i][4],
                       "comm_libelle" => $com_read[$i][5],
                       "comm_oper" => $com_read[$i][6],
                       "comm_expl" => $com_read[$i][7],
                       "comm_ref_igor" => $com_read[$i][8],
                       "comm_ref_rel" => $com_read[$i][9],
                       "comm_code_agence" => $codeAgence[0]
                    );
                }
            }

            

// --------------------------------------------------------------------------- PRINCIPALE -----------------------------------------------------------------


            $princ= $this->supprimerEspace($princ);
            $principalCI= $this->filtrerPrincCI($princ);
            $principalCO= $this->filtrerPrincCO($princ);
            $nonAuCI= $this->filtrerNonAuCI($princ);
            $nonAuCO= $this->filtrerNonAuCO($princ);
            $vi= $this->filtrerVi($princ);

            
            $principalCI= $this->trierPrincParRefIgor($principalCI);
            $principalCO= $this->trierParCleCroissante($principalCO);


// --------------------------------------------------------------------------- COMMISSION -----------------------------------------------------------------
            $comm= $this->supprimerEspace($comm);
            $commCI= $this->filtrerCommCI($comm);
            $commCI= $this->trierCommParRefIgor($commCI);

            $mergedComEtPrinc = $this->mergePrincEtCommi($principalCI, $commCI);
            $mergedComEtPrinc = $this->trierParCleCroissante($mergedComEtPrinc);


// ---------------------------------------------------------------------------- ORANGE -------------------------------------------------------------------------------
            $orange= $this->supprimerEspace($orange);
            $ind= $this->filtrerInd($orange);

            $orangeCI= $this->filtrerOrangeCI($orange);
            $orangeCI= $this->trierParCleCroissante($orangeCI);
            $orangeCO= $this->filtrerOrangeCO($orange);
            $orangeCO = $this->trierParCleCroissante($orangeCO);


//  ---------------------------------------------------------------------------- COMAPARAISON -------------------------------------------------------------------------------

            $resultatOrangeCI= $this->comparerBoaEtOrange($mergedComEtPrinc, $orangeCI);
            $resultatOrangeCO= $this->comparerBoaEtOrange($principalCO, $orangeCO);


// -----------------------------------------------------------------------------RESULTAT -------------------------------------------------------------------------------------

            $normalCI= $resultatOrangeCI[0];
            $normalCO= $resultatOrangeCO[0];
            $dat= $resultatOrangeCI[1];
            $cat= $resultatOrangeCO[1];
            $anomalieOrangeCI= $resultatOrangeCI[2];
            $anomalieOrangeCO= $resultatOrangeCO[2];


            // echo "<pre>";
            //     print_r("mergedComEtPrinc");
            // echo "</pre>";

            

          

// ------------------------------------------------------------------------------- HISTORIQUE -----------------------------------------------------------


            $historique= array_merge($normalCI[0], $normalCO[0], $dat[0], $cat[0], $anomalieOrangeCI[0], $anomalieOrangeCO[0], $ind, $nonAuCO, $nonAuCI, $vi);

            if(count($_SESSION["last_solde"]) > 0 ) {
                $solde_orange= $_SESSION["last_solde"][0]->solde_orange;
                $solde_boa= $_SESSION["last_solde"][0]->solde_boa;
            } else if(count($_SESSION["last_solde"]) === 0 ) {
                $solde_orange= 0;
                $solde_boa= 0;
            }
            
            
            foreach($historique as $item ) {
                $solde_orange += isset($item["solde"]) ? $item["solde"] : 0;
                $solde_boa += isset($item["MONTANT"]) ? $item["MONTANT"] : 0;
                $item["solde_orange"] = $solde_orange;
                $item["solde_boa"] = $solde_boa;
                $this->historique_orange_model->insert($item);
            }

// --------------------------------------------------------------------------------- REGULARISATION ------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------- INSERTION ----------------------------------------------------------------------------------
            $totalSteps = 10; 
            $currentStep = 0;

            $currentStep++; 
            $progressPercent = ($currentStep / $totalSteps) * 100;

            foreach($normalCI[0] as $item) {
                $this->orange_normal_model->insert_or_update_ci($item);
            }

            $responseData = array(
                'message' => 'Traitement en cours...',
                'progress' => $progressPercent
            );


            foreach($normalCO[0] as $item) {
                $this->orange_normal_model->insert_or_update_co($item);
            }

            $currentStep++; 
            $progressPercent = ($currentStep / $totalSteps) * 100;



            foreach ($dat[0] as $item ) {
                $this->boa_orange_anomalie_model->insert_or_update_dat($item);
            }

            $responseData = array(
                'message' => 'Traitement en cours...',
                'progress' => $progressPercent
            );


            foreach ($cat[0] as $item ) {
                $this->boa_orange_anomalie_model->insert_or_update_cat($item);
            }

            $currentStep++; 
            $progressPercent = ($currentStep / $totalSteps) * 100;

            foreach($ind as $item) {
                $this->orange_anomalie_model->insert_or_update_ind($item);
            }

            $responseData = array(
                'message' => 'Traitement en cours...',
                'progress' => $progressPercent
            );
            foreach($nonAuCI as $item) {
                $this->boa_orange_anomalie_model->insert_or_update_nonAuCI($item);
            }

            $currentStep++; 
            $progressPercent = ($currentStep / $totalSteps) * 100;

            foreach($nonAuCO as $item) {
                $this->boa_orange_anomalie_model->insert_or_update_nonAuCO($item);
            }

            $responseData = array(
                'message' => 'Traitement en cours...',
                'progress' => $progressPercent
            );

            // $this->exporter($normalCI[0], $normalCO[0], $dat[0], $cat[0], $ind, $vi, $nonAuCI, $nonAuCO);

            redirect("importer_orange");


        }

        public function filtrerOrangeCI($data) {
            $resultat= array();
            foreach ($data as $item) {
                if($item["orange_service"] ==="Cashin") {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerOrangeCO($data) {
            $resultat= array();
            foreach ($data as $item) {
                if($item["orange_service"] ==="CashOut") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }


        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension =="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

        }


        public function filtrerInd($data) {
            $resultat= array();

            foreach($data as $item) {
                if ($item["orange_num_compte"] ==="IND01") {
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function filtrerVI($data) {
            $resultat= array();

            foreach($data as $item) {
                if ($item["princ_oper"] !=="CASHI" && $item["princ_oper"] !=="CASHO" ) {
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function supprimerEspaceOrange($data) {
            foreach ($data as &$item) {
                foreach ($item as $key => &$value) {
                    $value = preg_replace('/\s+/', '', $value);
                }
                $item['orange_debit'] = preg_replace('/\s+/', '', $item['orange_debit']);
            }
            return $data;
        }


        public function filtrerPrincCI($data) {
            $resultat= array();

            foreach($data as $item) {
                if (preg_match('/tel:(\d+)/', $item["princ_libelle"], $matches)) {
                    $numeroTelephone = $matches[1];
                    // echo "Numéro de téléphone : " . $numeroTelephone;
                } else {
                    // echo "Aucun numéro de téléphone trouvé.";
                }
                if($item["princ_oper"] ==="CASHI" && $item["princ_expl"] ==="AU") {
                    $cle = $numeroTelephone .$item["princ_montant"];
                    $item["cle"] = $cle;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerPrincCO($data) {
            $resultat= array();

            foreach($data as $item) {
                if (preg_match('/tel:(\d+)/', $item["princ_libelle"], $matches)) {
                    $numeroTelephone = $matches[1];
                    // echo "Numéro de téléphone : " . $numeroTelephone;
                } else {
                    // echo "Aucun numéro de téléphone trouvé.";
                }
                if($item["princ_oper"] ==="CASHO" && $item["princ_expl"] ==="AU") {
                    $cle = $numeroTelephone .-$item["princ_montant"];
                    $item["cle"] = $cle;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerCommCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["comm_oper"] ==="CASHI" && $item["comm_expl"] ==="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerNonAuCI($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["princ_oper"] ==="CASHI" && $item["princ_expl"] !=="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
            
        }


        public function filtrerNonAuCO($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["princ_oper"] ==="CASHO" && $item["princ_expl"] !=="AU") {
                    $resultat[]= $item;
                }
            }
            return $resultat;
            
        }


        public function mergePrincEtCommi($principal, $commission) {
            $normalePrinc= array();
            $normaleComm= array();
            $mergedComEtPrinc= array();

            foreach($principal as $itemPrinc) {
                foreach($commission as $itemComm) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normalePrinc[] = $itemPrinc;
                    }
                }
            }
            foreach($commission as $itemComm) {
                foreach($principal as $itemPrinc) {
                    if($itemPrinc["princ_ref_igor"] === $itemComm["comm_ref_igor"]) {
                        $normaleComm[] = $itemComm;
                    }
                }
            }

            $ligne= count($normalePrinc) > count($normaleComm) ? $normalePrinc : $normaleComm;

            for ($i =0; $i < count($normalePrinc) ; $i++) {
                $mergedComEtPrinc[] = array_merge($normalePrinc[$i], $normaleComm[$i]);
            }

            return $mergedComEtPrinc;
        }

        public function trierCommParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['comm_ref_igor'], $b['comm_ref_igor']);
            });

            return $data;
        }

        public function trierPrincParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['princ_ref_igor'], $b['princ_ref_igor']);
            });

            return $data;
        }

        public function trierParCleCroissante($data){
            usort($data, function ($a, $b) {
                $cleA = intval($a['cle']);
                $cleB = intval($b['cle']);

                if ($cleA == $cleB) {
                    return 0;
                }
                return ($cleA < $cleB) ? -1 : 1;
            });

            return $data;
        }

        // public function comparerBoaEtOrange($boa, $orange) {
        //     $anomalieBoa = array();
        //     $anomalieOrange = array();
        //     $normaleOrange = array();
        //     $normaleBoa = array();
        //     $boaCopy = $boa;
        //     $orangeCopy = $orange;
        
        //     $ligne = min(count($boa), count($orange));
        //     $isFinished = false;
        //     $i = 0;
        
        //     while ($i < $ligne && !$isFinished) {
        //         if ($boa[$i]["cle"] - $orange[$i]["cle"] == 0) {
        //             $normaleOrange[] = $orange[$i];
        //             $normaleBoa[] = $boa[$i];
        //             $isFinished = true;
        //             $i++;
        //         } elseif ($boa[$i]["cle"] - $orange["cle"] < 0) {
        //             $anomalieBoa[] = $boa[$i];
        //             array_splice($boa, $i, 1);
        //             $boa = array_values($boa);
        //         } else {
        //             $anomalieOrange[] = $orange;
        //             array_splice($orange, $i, 1);
        //             $orange = array_values($orange);
        //         }
        //     }


        //     echo "<pre>";
        //     print_r();
        //     echo "</pre>";
        // }

        public function comparerBoaEtOrange ($igorTab, $orangeTab) {
            $orangeNormal = array();
            $boaNormal = array();
            $boaAnomalie = array();
            $orangeAnomalie = array();

            $orangeCount = count($orangeTab);
            $boaCount = count($igorTab);

            $i = 0;
            $j = 0;

            while ($i < $boaCount && $j < $orangeCount) {
                $cleIgor = $igorTab[$i]["cle"];
                $cleTelma = $orangeTab[$j]["cle"];
                $difference = $cleIgor - $cleTelma;

                if ($difference === 0) {
                    $orangeNormal[] = $orangeTab[$j];
                    $boaNormal[] = $igorTab[$i];
                    $i++;
                    $j++;
                } elseif ($difference < 0) {
                    $boaAnomalie[] = $igorTab[$i];
                    $i++;
                } else {
                    $orangeAnomalie[] = $orangeTab[$j];
                    $j++;
                }
            }

            // Traiter les éléments restants dans $igorTab comme des anomalies
            while ($i < $boaCount) {
                $boaAnomalie[] = $igorTab[$i];
                $i++;
            }

            // Traiter les éléments restants dans $orangeTab comme des anomalies
            while ($j < $orangeCount) {
                $orangeAnomalie[] = $orangeTab[$j];
                $j++;
            }


            for($i=0; $i < count($boaNormal); $i++) {
                $mergeBoaOrange[]= array_merge($boaNormal[$i], $orangeNormal[$i]);
            } 

            // echo "<pre>";
            //     print_r($mergeBoaOrange);
            // echo "</pre>";
          



            return [[$mergeBoaOrange], [$boaAnomalie], [$orangeAnomalie]];
        }

        public function exporter($normalCI, $normalCO, $dat, $cat, $ind, $vi, $nonAuCI, $nonAuCO) {
            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproOrange-" .$dateAujourdhui .".xlsx";

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

            // EN TETE 

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

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');

        }

        


        
}

?>