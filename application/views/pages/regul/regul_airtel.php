<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>
<div class="container-fluid mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabDat">Debit à tort</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabCat">Credit à tort</a>
        </li>
        <li class="nav-item col-sm-1">
            <a class="nav-link" data-toggle="tab" href="#tabMvts">MVTS</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabAmbigu">Ambigu</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabDeallo">Deallocation</a>
        </li>
        <li class="nav-item col-sm-1">
            <a class="nav-link" data-toggle="tab" href="#tabVi">Vi</a>
        </li>
    </ul>
</div>

<?php $this->load->view("pages/regul/airtel/tabDat"); ?>
<?php $this->load->view("pages/regul/airtel/tabCat"); ?>
<?php $this->load->view("pages/regul/airtel/tabMvts"); ?>
<?php $this->load->view("pages/regul/airtel/tabDeallo"); ?>
<?php $this->load->view("pages/regul/airtel/tabAmbigu"); ?>
<?php $this->load->view("pages/regul/airtel/tabVi"); ?>


  

<?php if (!empty($ambi)) { ?>
  <?php foreach($ambi as $item)  { ?>
    <div class="modal fade" id="myModalAirtel<?php echo $item->id_rollback; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h6 class="text-center">Infos</h6>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                  <div class="row">
                    <div class="col-sm-6">
                      <b>Régularisé</b>:
                      <?php
                        $dateObj = new DateTime($item->rollback_transfer_date);
                        $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                        $formattedDate2 = $formatter2->format($dateObj);
                        echo "le " .$formattedDate2 ?>
                    </div>
                    <div class="col-sm-6">
                    <b>Réf: </b><?php echo $item->rollback_reference_number; ?> </p>
                    </div>
                    <div class="col-sm-6">
                      <b>Service: </b><?php echo $item->rollback_service_name; ?> </p>
                    </div>
                    <div class="col-sm-6">
                      <b>Montant</b>: <?php echo $item->rollback_amount; ?> </p>
                    </div>
                    <div class="col-sm-5">
                    <b>Durée:</b>
                    <?php 
                        $date1 = new DateTime($item->rollback_transfer_date);
                        $date2 = new DateTime($item->transfer_date);
                        $interval = $date1->diff($date2);
                        $diffInDays = $interval->days;
                        echo $diffInDays ."Jours";
                      ?>
                    </div>
                  </div>
                </div>
            </div>
        </div>
    </div>
  <?php } ?>
  
<?php } ?>

<?php if (!empty($ci)) { ?>     
  <div class="modal fade" id="myModalBoa<?php echo $ci[0]->id_nonau; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
      <div class="modal-dialog" role="document">
          <div class="modal-content">
              <div class="modal-header">
                  <h6 class="text-center">Infos</h6>
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                      <span aria-hidden="true">&times;</span>
                  </button>
              </div>
              <div class="modal-body">
                <div class="row">
                  <div class="col-sm-7">
                    <b>Régularisé</b>: 
                    <?php
                      $dateObj = new DateTime($ci[0]->date_regul);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réf: </b><?php echo $ci[0]->ref_igor; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>opération</b>: <?php echo $ci[0]->oper; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($ci[0]->DATE_OPER);
                      $date2 = new DateTime($ci[0]->date_regul);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $ci[0]->montant ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?php 
                      echo $ci[0]->expl;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } ?>

<?php if (!empty($deallo_vi)) { ?>
  <?php foreach($deallo_vi as $item) { ?>
   
    <div class="modal fade" id="myModalDeallo<?php echo $item->REF_IGOR; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h6 class="text-center">Infos</h6>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                  <?php foreach($deallo_vi as $item) { ?>
                    <div class="row">
                    <div class="col-sm-7">
                      <b>Régularisé</b>: <?php echo $item->TRANSFER_ID;  ?>
                    </div>
                    <div class="col-sm-5">
                      <b>Service: </b>: <?php echo $item->service_name;  ?></p>
                    </div>
                    <div class="col-sm-7">
                      <b>Réference</b>: <?php echo $item->reference_number;  ?></p>
                    </div>
                    <div class="col-sm-5">
                    <b>Durée:</b>
                   
                    </div>
                    <div class="col-sm-7">
                      <b> Montant: </b> <?php echo $item->amount;  ?>
                    </div>
                    <div class="col-sm-5">
                    <b> Exploitant: </b>
                    </div>
                  </div>
                  <?php } ?>

                  
                </div>
            </div>
        </div>
    </div>
  <?php } ?>
<?php } ?>

<script>
  let minDateBoa, maxDateBoa;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minBoa = minDateBoa.val();
        let max = maxDateBoa.val();
        let date = new Date(data[1]);
    
        if (
            (minBoa === null && max === null) ||
            (minBoa === null && date <= max) ||
            (minBoa <= date && max === null) ||
            (minBoa <= date && date <= max)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDateBoa = new DateTime('#minBoa', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateBoa = new DateTime('#maxBoa', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let tableBoa = new DataTable('#tableBoa', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minBoa, #maxBoa').forEach((el) => {
        el.addEventListener('change', () => tableBoa.draw());
    });
</script>

<script>
  let minDateAirtel, maxDateAirtel;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minAirtel = minDateAirtel.val();
        let maxAirtel = maxDateAirtel.val();
        let date = new Date(data[1]);
    
        if (
            (minAirtel === null && maxAirtel === null) ||
            (minAirtel === null && date <= maxAirtel) ||
            (minAirtel <= date && maxAirtel === null) ||
            (minAirtel <= date && date <= maxAirtel)
        ) {
            return true;
        }
        return false;
    });
    
    minDateAirtel = new DateTime('#minAirtel', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateAirtel = new DateTime('#maxAirtel', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    let tableAirtel = new DataTable('#tableAirtel', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minAirtel, #maxAirtel').forEach((el) => {
        el.addEventListener('change', () => tableAirtel.draw());
    });
</script>
<script>



$("#tableDat").DataTable({
    scrollX: true,
    autoFill: true,
    responsive: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableCat").DataTable({
    scrollX: true,
    autoFill: true,
    responsive: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableMvts").DataTable({
    scrollX: true,
    autoFill: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableAmbigu").DataTable({
    scrollX: true,
    autoFill: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableAmbiguCO").DataTable({
    scrollX: true,
    autoFill: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableDeallo").DataTable({
    scrollX: true,
    autoFill: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});
$("#tableVi").DataTable({
    scrollX: true,
    autoFill: true,
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});

</script>