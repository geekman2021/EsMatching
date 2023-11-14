<div class=" container-fluid mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-3">
            <a class="nav-link" data-toggle="tab" href="#tabDat">Debit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
            <a class="nav-link" data-toggle="tab" href="#tabCat">Credit à tort</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabMvts">MVTS</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabInd">IND</a>
        </li>
        <li class="nav-item col-sm-2">
            <a class="nav-link" data-toggle="tab" href="#tabVi">VI</a>
        </li>
    </ul>
</div>

<?php $this->load->view("pages/regul/orange/tabDat"); ?>
<?php $this->load->view("pages/regul/orange/tabCat"); ?>
<?php $this->load->view("pages/regul/orange/tabInd"); ?>
<?php $this->load->view("pages/regul/orange/tabMvts"); ?>
<?php $this->load->view("pages/regul/orange/tabVi"); ?>


<script>
        $(document).ready(function(e) {
        $("#tableDat").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableCat").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableInd").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableMvts").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
        $("#tableVi").DataTable({
            language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
        });
    });
</script>





